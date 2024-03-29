/* passwd.c: Support for the passwd database.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/service/nss/passwd.h>

srvd_boolean_t srvd_service_nss_passwd_request_name_get(const srvd_service_request_t *request,
                                                        char **name) {
  srvd_protocol_packet_field_t *field = NULL;
  srvd_protocol_packet_field_entry_t *entry = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(request->packet.field_count > 0);

  SRVD_RETURN_FALSE_UNLESS(name);
  SRVD_RETURN_FALSE_UNLESS(*name == NULL);

  srvd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != SRVD_SERVICE_NSS_PASSWD_REQUEST_NAME) {
    SRVD_LOG_ERROR("srvd_service_nss_passwd_request_name_get: Invalid packet type");
    return SRVD_FALSE;
  }

  SRVD_RETURN_FALSE_UNLESS(field->entry_count == 1);

  srvd_protocol_packet_field_entry_get_first(field, &entry);

  *name = malloc(entry->size);
  if(*name == NULL) {
    SRVD_LOG_ERROR("srvd_service_nss_passwd_request_name_get: Unable to allocate memory for name");
    return SRVD_FALSE;
  }

  strncpy(*name, entry->data, entry->size);
  (*name)[entry->size - 1] = '\0';

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_nss_passwd_request_name_free(const srvd_service_request_t *request,
                                                         char **name) {
  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(name);
  SRVD_RETURN_FALSE_UNLESS(*name);

  free(*name);
  *name = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_nss_passwd_request_uid_get(const srvd_service_request_t *request,
                                                       uid_t *uid) {
  srvd_protocol_packet_field_t *field = NULL;
  srvd_protocol_packet_field_entry_t *entry = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(request->packet.field_count > 0);

  SRVD_RETURN_FALSE_UNLESS(uid);

  srvd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != SRVD_SERVICE_NSS_PASSWD_REQUEST_UID) {
    SRVD_LOG_ERROR("srvd_service_nss_passwd_request_uid_get: Invalid packet type");
    return SRVD_FALSE;
  }

  SRVD_RETURN_FALSE_UNLESS(field->entry_count == 1);

  srvd_protocol_packet_field_entry_get_first(field, &entry);

  return srvd_protocol_packet_field_entry_get_uint32(entry, (uint32_t *)uid);
}

srvd_boolean_t srvd_service_nss_passwd_request_entities_get(const srvd_service_request_t *request,
                                                            int32_t *offset) {
  srvd_protocol_packet_field_t *field = NULL;
  srvd_protocol_packet_field_entry_t *entry = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(request->packet.field_count > 0);

  SRVD_RETURN_FALSE_UNLESS(offset);

  srvd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != SRVD_SERVICE_NSS_PASSWD_REQUEST_ENTITIES) {
    SRVD_LOG_ERROR("srvd_service_nss_passwd_request_entites_get: Invalid packet type");
    return SRVD_FALSE;
  }

  SRVD_RETURN_FALSE_UNLESS(field->entry_count == 1);

  srvd_protocol_packet_field_entry_get_first(field, &entry);

  return srvd_protocol_packet_field_entry_get_uint32(entry, (uint32_t *)offset);
}

srvd_boolean_t srvd_service_nss_passwd_response_name_set(srvd_service_response_t *response,
                                                         const char *name, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(name);

  return srvd_protocol_packet_field_append(&response->packet,
                                           SRVD_SERVICE_NSS_PASSWD_RESPONSE_NAME, length, name);
}

srvd_boolean_t srvd_service_nss_passwd_response_uid_set(srvd_service_response_t *response,
                                                        uid_t uid) {
  SRVD_RETURN_FALSE_UNLESS(response);

  return srvd_protocol_packet_field_append_uint32(&response->packet,
                                                  SRVD_SERVICE_NSS_PASSWD_RESPONSE_UID,
                                                  (uint32_t)uid);
}

srvd_boolean_t srvd_service_nss_passwd_response_gid_set(srvd_service_response_t *response,
                                                        gid_t gid) {
  SRVD_RETURN_FALSE_UNLESS(response);

  return srvd_protocol_packet_field_append_uint32(&response->packet,
                                                  SRVD_SERVICE_NSS_PASSWD_RESPONSE_GID,
                                                  (uint32_t)gid);
}

srvd_boolean_t srvd_service_nss_passwd_response_dir_set(srvd_service_response_t *response,
                                                        const char *dir, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(dir);

  return srvd_protocol_packet_field_append(&response->packet, SRVD_SERVICE_NSS_PASSWD_RESPONSE_DIR,
                                           length, dir);
}

srvd_boolean_t srvd_service_nss_passwd_response_shell_set(srvd_service_response_t *response,
                                                          const char *shell, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(shell);

  return srvd_protocol_packet_field_append(&response->packet,
                                           SRVD_SERVICE_NSS_PASSWD_RESPONSE_SHELL, length, shell);
}

srvd_boolean_t srvd_service_nss_passwd_response_gecos_set(srvd_service_response_t *response,
                                                          const char *gecos, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(gecos);

  return srvd_protocol_packet_field_append(&response->packet,
                                           SRVD_SERVICE_NSS_PASSWD_RESPONSE_GECOS, length, gecos);
}
