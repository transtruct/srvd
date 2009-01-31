/* passwd.c: Name switch service for usernames.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include "passwd.h"

#include <nssd/nssd.h>
#include <nssd/buffer.h>
#include <nssd/service.h>
#include <nssd/service/passwd.h>
#include <nssd/protocol/packet.h>
#include <nssd/protocol/request.h>
#include <nssd/protocol/response.h>

#include <arpa/inet.h>

#include <stdio.h>

static enum nss_status _nssd_nss_passwd_callback(const nssd_protocol_response_t *response,
                                                 struct passwd *pwd, char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status = NSS_STATUS_SUCCESS;
  NSSD_SERVICE_RESP_FIELD(field);
  NSSD_BUFFER(bi, buffer);

  NSSD_SERVICE_RESP_ITERATE(response, field) {
    switch(NSSD_SERVICE_RESP_FIELD_TYPE(response, field)) {
    case NSSD_SERVICE_PASSWD_RESP_NAME:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize,
                                   NSSD_SERVICE_RESP_FIELD_LENGTH(response, field))) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }
      
      NSSD_SERVICE_RESP_FIELD_GET_STRING(NSSD_BUFFER_REF(bi), response, field);
      pwd->pw_name = NSSD_BUFFER_REF(bi);
      
      NSSD_BUFFER_ITERATOR_NEXT(bi,
                                NSSD_SERVICE_RESP_FIELD_LENGTH(response, field));
      break;
      
    case NSSD_SERVICE_PASSWD_RESP_UID:
      pwd->pw_uid = (uid_t)NSSD_SERVICE_RESP_FIELD_GET_UINT32(response, field);
      
      break;
      
    case NSSD_SERVICE_PASSWD_RESP_GID:
      pwd->pw_gid = (gid_t)NSSD_SERVICE_RESP_FIELD_GET_UINT32(response, field);
      
      break;
      
    case NSSD_SERVICE_PASSWD_RESP_DIR:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize,
                                   NSSD_SERVICE_RESP_FIELD_LENGTH(response, field))) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }
      
      NSSD_SERVICE_RESP_FIELD_GET_STRING(NSSD_BUFFER_REF(bi), response, field);
      pwd->pw_dir = NSSD_BUFFER_REF(bi);
      
      NSSD_BUFFER_ITERATOR_NEXT(bi,
                                NSSD_SERVICE_RESP_FIELD_LENGTH(response, field));
      break;
      
    case NSSD_SERVICE_PASSWD_RESP_SHELL:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize,
                                   NSSD_SERVICE_RESP_FIELD_LENGTH(response, field))) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }
      
      NSSD_SERVICE_RESP_FIELD_GET_STRING(NSSD_BUFFER_REF(bi), response, field);
      pwd->pw_shell = NSSD_BUFFER_REF(bi);
      
      NSSD_BUFFER_ITERATOR_NEXT(bi,
                                NSSD_SERVICE_RESP_FIELD_LENGTH(response, field));
      break;
      
    default:
      break;
    }
  }
  
 _nssd_nss_passwd_callback_error:

  return status;
} 

enum nss_status
_nss_daemon_getpwnam_r(const char *name, struct passwd *pwd,
                       char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status;
  nssd_protocol_packet_t packet;
  nssd_protocol_response_t response;

  nssd_protocol_packet_initialize(&packet);
  NSSD_SERVICE_REQ_SIMPLE_STRING(&packet, NSSD_SERVICE_PASSWD_REQ_NAME,
                                 name, sysconf(_SC_LOGIN_NAME_MAX) + 1);

  nssd_protocol_response_initialize(&response);

  nssd_protocol_request(&packet, &response);
  nssd_protocol_packet_finalize(&packet);

  if(response.status == NSSD_PROTOCOL_RESPONSE_NOTFOUND) {
    NSSD_NSS_NORECORD(status, _nss_daemon_getpwnam_r_error);
  }
  else if(response.status == NSSD_PROTOCOL_RESPONSE_UNAVAIL) {
    NSSD_NSS_UNAVAIL(status, _nss_daemon_getpwnam_r_error);
  }
  else if(response.status != NSSD_PROTOCOL_RESPONSE_SUCCESS) {
    NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_daemon_getpwnam_r_error);
  }

  status = _nssd_nss_passwd_callback(&response, pwd, buffer, bufsize, ret_errno);

 _nss_daemon_getpwnam_r_error:

  nssd_protocol_response_finalize(&response);

  return status;
}

enum nss_status
_nss_daemon_getpwuid_r(uid_t uid, struct passwd *pwd,
                       char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status;
  nssd_protocol_packet_t packet;
  nssd_protocol_response_t response;

  nssd_protocol_packet_initialize(&packet);
  NSSD_SERVICE_REQ_SIMPLE_UINT32(&packet, NSSD_SERVICE_PASSWD_REQ_UID, uid);

  nssd_protocol_response_initialize(&response);

  nssd_protocol_request(&packet, &response);
  nssd_protocol_packet_finalize(&packet);

  if(response.status == NSSD_PROTOCOL_RESPONSE_NOTFOUND) {
    NSSD_NSS_NORECORD(status, _nss_daemon_getpwuid_r_error);
  }
  else if(response.status == NSSD_PROTOCOL_RESPONSE_UNAVAIL) {
    NSSD_NSS_UNAVAIL(status, _nss_daemon_getpwuid_r_error);
  }
  else if(response.status != NSSD_PROTOCOL_RESPONSE_SUCCESS) {
    NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_daemon_getpwuid_r_error);
  }

  status = _nssd_nss_passwd_callback(&response, pwd, buffer, bufsize, ret_errno);

 _nss_daemon_getpwuid_r_error:

  nssd_protocol_response_finalize(&response);

  return status;
}

enum nss_status
_nss_daemon_setpwent(int stayopen) {
  NSSD_UNUSED(stayopen);
  
  return NSS_STATUS_UNAVAIL;
}


enum nss_status
_nss_daemon_endpwent(void) {
  return NSS_STATUS_UNAVAIL;
}

enum nss_status
_nss_daemon_getpwent_r(struct passwd *pwd, char *buffer,
                       size_t bufsize, int *ret_errno) {
  printf("getpwent()\n");

  return NSS_STATUS_UNAVAIL;
}
