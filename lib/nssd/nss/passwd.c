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
#include <nssd/protocol/packet.h>
#include <nssd/service.h>
#include <nssd/service/passwd.h>
#include <nssd/thread.h>

#include <stdio.h>

static enum nss_status _nssd_nss_passwd_callback(const nssd_service_response_t *response,
                                                 struct passwd *pwd, char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status = NSS_STATUS_SUCCESS;
  nssd_protocol_packet_field_t *field;
  NSSD_BUFFER(bi, buffer);

  NSSD_SERVICE_RESPONSE_FIELD_ITERATE(response, field) {
    switch(field->type) {
    case NSSD_SERVICE_PASSWD_RESPONSE_NAME:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, field->size)) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }

      strncpy(NSSD_BUFFER_REF(bi), field->data, field->size);
      NSSD_BUFFER_REF(bi)[field->size - 1] = '\0';
      pwd->pw_name = NSSD_BUFFER_REF(bi);

      NSSD_BUFFER_ITERATOR_NEXT(bi, field->size);
      break;

    case NSSD_SERVICE_PASSWD_RESPONSE_UID:
      nssd_protocol_packet_field_get_uint32(field, &pwd->pw_uid);
      break;

    case NSSD_SERVICE_PASSWD_RESPONSE_GID:
      nssd_protocol_packet_field_get_uint32(field, &pwd->pw_gid);
      break;

    case NSSD_SERVICE_PASSWD_RESPONSE_DIR:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, field->size)) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }

      strncpy(NSSD_BUFFER_REF(bi), field->data, field->size);
      NSSD_BUFFER_REF(bi)[field->size - 1] = '\0';
      pwd->pw_dir = NSSD_BUFFER_REF(bi);

      NSSD_BUFFER_ITERATOR_NEXT(bi, field->size);
      break;

    case NSSD_SERVICE_PASSWD_RESPONSE_SHELL:
      if(!NSSD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, field->size)) {
        NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _nssd_nss_passwd_callback_error);
      }

      strncpy(NSSD_BUFFER_REF(bi), field->data, field->size);
      NSSD_BUFFER_REF(bi)[field->size - 1] = '\0';
      pwd->pw_shell = NSSD_BUFFER_REF(bi);

      NSSD_BUFFER_ITERATOR_NEXT(bi, field->size);
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
  nssd_service_request_t request;
  nssd_service_response_t response;

  nssd_service_request_initialize(&request);
  nssd_protocol_packet_field_add(&request.packet, NSSD_SERVICE_PASSWD_REQUEST_NAME,
                                 (uint16_t)(sysconf(_SC_LOGIN_NAME_MAX) + 1), name);

  nssd_service_response_initialize(&response);
  nssd_service_request_query(&request, &response);

  if(response.status == NSSD_SERVICE_RESPONSE_NOTFOUND) {
    NSSD_NSS_NORECORD(status, _nss_daemon_getpwnam_r_error);
  }
  else if(response.status == NSSD_SERVICE_RESPONSE_UNAVAIL) {
    NSSD_NSS_UNAVAIL(status, _nss_daemon_getpwnam_r_error);
  }
  else if(response.status != NSSD_SERVICE_RESPONSE_SUCCESS) {
    NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_daemon_getpwnam_r_error);
  }

  status = _nssd_nss_passwd_callback(&response, pwd, buffer, bufsize, ret_errno);

 _nss_daemon_getpwnam_r_error:

  nssd_service_request_finalize(&request);
  nssd_service_response_finalize(&response);

  return status;
}

enum nss_status
_nss_daemon_getpwuid_r(uid_t uid, struct passwd *pwd,
                       char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status;
  nssd_service_request_t request;
  nssd_service_response_t response;

  nssd_service_request_initialize(&request);
  nssd_protocol_packet_field_add_uint32(&request.packet, NSSD_SERVICE_PASSWD_REQUEST_UID, uid);

  nssd_service_response_initialize(&response);
  nssd_service_request_query(&request, &response);

  if(response.status == NSSD_SERVICE_RESPONSE_NOTFOUND) {
    NSSD_NSS_NORECORD(status, _nss_daemon_getpwuid_r_error);
  }
  else if(response.status == NSSD_SERVICE_RESPONSE_UNAVAIL) {
    NSSD_NSS_UNAVAIL(status, _nss_daemon_getpwuid_r_error);
  }
  else if(response.status != NSSD_SERVICE_RESPONSE_SUCCESS) {
    NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_daemon_getpwuid_r_error);
  }

  status = _nssd_nss_passwd_callback(&response, pwd, buffer, bufsize, ret_errno);

 _nss_daemon_getpwuid_r_error:

  nssd_service_request_finalize(&request);
  nssd_service_response_finalize(&response);

  return status;
}

static NSSD_THREAD_ONCE_DECLARE(_nssd_nss_passwd_pwent_initialize);
static NSSD_THREAD_KEY_DECLARE(_nssd_nss_passwd_pwent_offset);

static void _nssd_nss_passwd_pwent_initialize_callback(void) {
  NSSD_THREAD_KEY_INITIALIZE(_nssd_nss_passwd_pwent_offset);
}

static nssd_boolean_t _nssd_nss_passwd_pwent_initialize_offset(void) {
  uint32_t *offset = malloc(sizeof(uint32_t));
  if(offset == NULL) {
    NSSD_LOG_ERROR("Unable to allocate memory for offset variable");
    return NSSD_FALSE;
  }

  (*offset) = 0;
  NSSD_THREAD_KEY_DATA_SET(_nssd_nss_passwd_pwent_offset, offset);

  return NSSD_TRUE;
}

enum nss_status
_nss_daemon_setpwent(int stayopen) {
  uint32_t *offset;

  NSSD_UNUSED(stayopen);

  NSSD_THREAD_ONCE_CALL(_nssd_nss_passwd_pwent_initialize,
                        _nssd_nss_passwd_pwent_initialize_callback);
  if(!NSSD_THREAD_KEY_DATA_HAS(_nssd_nss_passwd_pwent_offset)) {
    if(!_nssd_nss_passwd_pwent_initialize_offset())
      return NSS_STATUS_UNAVAIL;
  }

  offset = (uint32_t *)NSSD_THREAD_KEY_DATA_GET(_nssd_nss_passwd_pwent_offset);
  *offset = 0;

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_daemon_endpwent(void) {
  NSSD_THREAD_ONCE_CALL(_nssd_nss_passwd_pwent_initialize,
                        _nssd_nss_passwd_pwent_initialize_callback);
  if(NSSD_THREAD_KEY_DATA_HAS(_nssd_nss_passwd_pwent_offset)) {
    uint32_t *offset = (uint32_t *)NSSD_THREAD_KEY_DATA_GET(_nssd_nss_passwd_pwent_offset);
    NSSD_THREAD_KEY_DATA_SET(_nssd_nss_passwd_pwent_offset, NULL);
    free(offset);
  }

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_daemon_getpwent_r(struct passwd *pwd, char *buffer,
                       size_t bufsize, int *ret_errno) {
  enum nss_status status;
  uint32_t *offset;

  nssd_service_request_t request;
  nssd_service_response_t response;

  NSSD_THREAD_ONCE_CALL(_nssd_nss_passwd_pwent_initialize,
                        _nssd_nss_passwd_pwent_initialize_callback);
  if(!NSSD_THREAD_KEY_DATA_HAS(_nssd_nss_passwd_pwent_offset)) {
    if(!_nssd_nss_passwd_pwent_initialize_offset())
      return NSS_STATUS_UNAVAIL;
  }

  offset = (uint32_t *)NSSD_THREAD_KEY_DATA_GET(_nssd_nss_passwd_pwent_offset);

  nssd_service_request_initialize(&request);
  nssd_protocol_packet_field_add_uint32(&request.packet, NSSD_SERVICE_PASSWD_REQUEST_ENTITIES,
                                        *offset);

  nssd_service_response_initialize(&response);
  nssd_service_request_query(&request, &response);

  if(response.status == NSSD_SERVICE_RESPONSE_NOTFOUND) {
    NSSD_NSS_NORECORD(status, _nss_daemon_getpwent_r_error);
  }
  else if(response.status == NSSD_SERVICE_RESPONSE_UNAVAIL) {
    NSSD_NSS_UNAVAIL(status, _nss_daemon_getpwent_r_error);
  }
  else if(response.status != NSSD_SERVICE_RESPONSE_SUCCESS) {
    NSSD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_daemon_getpwent_r_error);
  }

  status = _nssd_nss_passwd_callback(&response, pwd, buffer, bufsize, ret_errno);
  (*offset)++;

 _nss_daemon_getpwent_r_error:

  nssd_service_request_finalize(&request);
  nssd_service_response_finalize(&response);

  return status;
}
