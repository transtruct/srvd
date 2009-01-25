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
#include <nssd/service/passwd.h>
#include <nssd/protocol/packet.h>
#include <nssd/protocol/request.h>
#include <nssd/protocol/response.h>

#include <arpa/inet.h>

#include <stdio.h>

enum nss_status
_nss_daemon_getpwnam_r(const char *name, struct passwd *pwd,
                       char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status;
  nssd_protocol_packet_t packet;
  nssd_protocol_response_t response;

  nssd_protocol_packet_initialize(&packet);
  nssd_protocol_packet_fields_initialize(&packet, 1);

  packet.fields[0].type = NSSD_SERVICE_PASSWD_REQ_NAME;
  packet.fields[0].length = sysconf(_SC_LOGIN_NAME_MAX) + 1;

  nssd_protocol_packet_field_initialize(&packet, 0);
  memcpy(packet.fields[0].data, (void *)name, packet.fields[0].length);

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

  status = NSS_STATUS_SUCCESS;

  {
    int i;
    NSSD_BUFFER(bi, buffer);
    for(i = 0; i < response.packet.field_count; i++) {
      switch(response.packet.fields[i].type) {
      case NSSD_SERVICE_PASSWD_RESP_NAME:
        if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, response.packet.fields[i].length)) {
          NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                        _nss_daemon_getpwnam_r_error);
        }

        memcpy(NSSD_BUFFER_REF(bi),
               response.packet.fields[i].data, response.packet.fields[i].length);
        pwd->pw_name = NSSD_BUFFER_REF(bi);

        NSSD_BUFFER_ITERATOR_NEXT(bi, response.packet.fields[i].length);
        break;

      case NSSD_SERVICE_PASSWD_RESP_UID:
        assert(response.packet.fields[i].length == sizeof(uint32_t));

        pwd->pw_uid = (uid_t)ntohl(*(uint32_t *)response.packet.fields[i].data);

        break;

      case NSSD_SERVICE_PASSWD_RESP_GID:
        assert(response.packet.fields[i].length == sizeof(uint32_t));

        pwd->pw_gid = (gid_t)ntohl(*(uint32_t *)response.packet.fields[i].data);

        break;

      case NSSD_SERVICE_PASSWD_RESP_DIR:
        if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, response.packet.fields[i].length)) {
          NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                        _nss_daemon_getpwnam_r_error);
        }

        memcpy(NSSD_BUFFER_REF(bi),
               response.packet.fields[i].data, response.packet.fields[i].length);
        pwd->pw_dir = NSSD_BUFFER_REF(bi);

        NSSD_BUFFER_ITERATOR_NEXT(bi, response.packet.fields[i].length);
        break;

      case NSSD_SERVICE_PASSWD_RESP_SHELL:
        if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, response.packet.fields[i].length)) {
          NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                        _nss_daemon_getpwnam_r_error);
        }

        memcpy(NSSD_BUFFER_REF(bi),
               response.packet.fields[i].data, response.packet.fields[i].length);
        pwd->pw_shell = NSSD_BUFFER_REF(bi);

        NSSD_BUFFER_ITERATOR_NEXT(bi, response.packet.fields[i].length);
        break;

      default:
        break;
      }
    }
  }

 _nss_daemon_getpwnam_r_error:

  nssd_protocol_response_finalize(&response);

  return status;
}

enum nss_status
_nss_daemon_getpwuid_r(uid_t uid, struct passwd *pwd,
                       char *buffer, size_t bufsize, int *ret_errno) {
  printf("getpwuid()\n");

  return NSS_STATUS_UNAVAIL;
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
