/* aliases.c: Name switch service for mail aliases.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include "aliases.h"

#include <srvd/srvd.h>
#include <srvd/buffer.h>
#include <srvd/protocol/packet.h>
#include <srvd/service.h>
#include <srvd/service/nss/aliases.h>
#include <srvd/thread.h>

#ifdef HAVE_ALIASES
static enum nss_status _srvd_nss_aliases_populate(const srvd_service_response_t *response,
                                                  struct aliasent *ae, char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status = NSS_STATUS_SUCCESS;
  srvd_protocol_packet_field_t *field;
  SRVD_BUFFER(bi, buffer);

  /* Servers are not required to send the LOCAL field. */
  ae->alias_local = 0;

  SRVD_SERVICE_RESPONSE_FIELD_ITERATE(response, field) {
    srvd_protocol_packet_field_entry_t *entry = NULL;

    switch(field->type) {
    case SRVD_SERVICE_NSS_ALIASES_RESPONSE_NAME:
      if(!srvd_protocol_packet_field_entry_get_first(field, &entry)) {
        SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                      _srvd_nss_aliases_populate_error);
      }

      if(!SRVD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, entry->size)) {
        SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _srvd_nss_aliases_populate_error);
      }

      strncpy(SRVD_BUFFER_REF(bi), entry->data, entry->size);
      SRVD_BUFFER_REF(bi)[entry->size - 1] = '\0';
      ae->alias_name = SRVD_BUFFER_REF(bi);

      SRVD_BUFFER_ITERATOR_NEXT(bi, entry->size);
      break;

    case SRVD_SERVICE_NSS_ALIASES_RESPONSE_MEMBERS:
      ae->alias_members_len = field->entry_count;

      if(!SRVD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, ae->alias_members_len * sizeof(char *))) {
        SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                      _srvd_nss_aliases_populate_error);
      }

      memset(SRVD_BUFFER_REF(bi), 0, ae->alias_members_len * sizeof(char *));
      ae->alias_members = (char **)SRVD_BUFFER_REF(bi);

      SRVD_BUFFER_ITERATOR_NEXT(bi, ae->alias_members_len * sizeof(char *));

      uint16_t entry_offset;
      SRVD_PROTOCOL_PACKET_FIELD_ENTRY_ITERATE_COUNT(field, entry, entry_offset) {
        if(!SRVD_BUFFER_CHECK_OFFSET(bi, buffer, bufsize, entry->size)) {
          SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, ERANGE,
                        _srvd_nss_aliases_populate_error);
        }

        strncpy(SRVD_BUFFER_REF(bi), entry->data, entry->size);
        SRVD_BUFFER_REF(bi)[entry->size - 1] = '\0';
        ae->alias_members[entry_offset] = SRVD_BUFFER_REF(bi);

        SRVD_BUFFER_ITERATOR_NEXT(bi, entry->size);
      }
      break;

    case SRVD_SERVICE_NSS_ALIASES_RESPONSE_LOCAL:
      if(!srvd_protocol_packet_field_entry_get_first(field, &entry)) {
        SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                      _srvd_nss_aliases_populate_error);
      }

      srvd_protocol_packet_field_entry_get_uint8(entry, (uint8_t *)&ae->alias_local);
      break;

    default:
      break;
    }
  }

 _srvd_nss_aliases_populate_error:

  return status;
}

enum nss_status
_nss_srvd_getaliasbyname_r(const char *name, struct aliasent *ae,
                           char *buffer, size_t bufsize, int *ret_errno) {
  enum nss_status status;
  srvd_service_request_t request;
  srvd_service_response_t response;

  srvd_service_request_initialize(&request);
  srvd_protocol_packet_field_append(&request.packet, SRVD_SERVICE_NSS_ALIASES_REQUEST_NAME,
                                    (uint16_t)(sysconf(_SC_LOGIN_NAME_MAX) + 1), name);

  srvd_service_response_initialize(&response);
  srvd_service_request_query(&request, &response);

  if(response.status == SRVD_SERVICE_RESPONSE_NOTFOUND) {
    SRVD_NSS_NORECORD(status, _nss_srvd_getaliasbyname_r_error);
  }
  else if(response.status == SRVD_SERVICE_RESPONSE_UNAVAIL) {
    SRVD_NSS_UNAVAIL(status, _nss_srvd_getaliasbyname_r_error);
  }
  else if(response.status != SRVD_SERVICE_RESPONSE_SUCCESS) {
    SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_srvd_getaliasbyname_r_error);
  }

  status = _srvd_nss_aliases_populate(&response, ae, buffer, bufsize, ret_errno);

 _nss_srvd_getaliasbyname_r_error:

  srvd_service_request_finalize(&request);
  srvd_service_response_finalize(&response);

  return status;
}

static SRVD_THREAD_ONCE_DECLARE(_srvd_nss_aliases_aliasent_initialize);
static SRVD_THREAD_KEY_DECLARE(_srvd_nss_aliases_aliasent_offset);

static void _srvd_nss_aliases_aliasent_initialize_callback(void) {
  SRVD_THREAD_KEY_INITIALIZE(_srvd_nss_aliases_aliasent_offset);
}

static srvd_boolean_t _srvd_nss_aliases_aliasent_initialize_offset(void) {
  uint32_t *offset = malloc(sizeof(uint32_t));
  if(offset == NULL) {
    SRVD_LOG_ERROR("Unable to allocate memory for offset variable");
    return SRVD_FALSE;
  }

  (*offset) = 0;
  SRVD_THREAD_KEY_DATA_SET(_srvd_nss_aliases_aliasent_offset, offset);

  return SRVD_TRUE;
}

enum nss_status
_nss_srvd_setaliasent(int stayopen) {
  uint32_t *offset;

  SRVD_UNUSED(stayopen);

  SRVD_THREAD_ONCE_CALL(_srvd_nss_aliases_aliasent_initialize,
                        _srvd_nss_aliases_aliasent_initialize_callback);
  if(!SRVD_THREAD_KEY_DATA_HAS(_srvd_nss_aliases_aliasent_offset)) {
    if(!_srvd_nss_aliases_aliasent_initialize_offset())
      return NSS_STATUS_UNAVAIL;
  }

  offset = (uint32_t *)SRVD_THREAD_KEY_DATA_GET(_srvd_nss_aliases_aliasent_offset);
  *offset = 0;

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_srvd_endaliasent(void) {
  SRVD_THREAD_ONCE_CALL(_srvd_nss_aliases_aliasent_initialize,
                        _srvd_nss_aliases_aliasent_initialize_callback);
  if(SRVD_THREAD_KEY_DATA_HAS(_srvd_nss_aliases_aliasent_offset)) {
    uint32_t *offset = (uint32_t *)SRVD_THREAD_KEY_DATA_GET(_srvd_nss_aliases_aliasent_offset);
    SRVD_THREAD_KEY_DATA_SET(_srvd_nss_aliases_aliasent_offset, NULL);
    free(offset);
  }

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_srvd_getaliasent_r(struct aliasent *ae, char *buffer,
                        size_t bufsize, int *ret_errno) {
  enum nss_status status;
  uint32_t *offset;

  srvd_service_request_t request;
  srvd_service_response_t response;

  SRVD_THREAD_ONCE_CALL(_srvd_nss_aliases_aliasent_initialize,
                        _srvd_nss_aliases_aliasent_initialize_callback);
  if(!SRVD_THREAD_KEY_DATA_HAS(_srvd_nss_aliases_aliasent_offset)) {
    if(!_srvd_nss_aliases_aliasent_initialize_offset())
      return NSS_STATUS_UNAVAIL;
  }

  offset = (uint32_t *)SRVD_THREAD_KEY_DATA_GET(_srvd_nss_aliases_aliasent_offset);

  srvd_service_request_initialize(&request);
  srvd_protocol_packet_field_append_uint32(&request.packet,
                                           SRVD_SERVICE_NSS_ALIASES_REQUEST_ENTITIES, *offset);

  srvd_service_response_initialize(&response);
  srvd_service_request_query(&request, &response);

  if(response.status == SRVD_SERVICE_RESPONSE_NOTFOUND) {
    SRVD_NSS_NORECORD(status, _nss_srvd_getaliasent_r_error);
  }
  else if(response.status == SRVD_SERVICE_RESPONSE_UNAVAIL) {
    SRVD_NSS_UNAVAIL(status, _nss_srvd_getaliasent_r_error);
  }
  else if(response.status != SRVD_SERVICE_RESPONSE_SUCCESS) {
    SRVD_NSS_FAIL(status, ret_errno, NSS_STATUS_TRYAGAIN, EINVAL,
                  _nss_srvd_getaliasent_r_error);
  }

  status = _srvd_nss_aliases_populate(&response, ae, buffer, bufsize, ret_errno);
  (*offset)++;

 _nss_srvd_getaliasent_r_error:

  srvd_service_request_finalize(&request);
  srvd_service_response_finalize(&response);

  return status;
}
#endif
