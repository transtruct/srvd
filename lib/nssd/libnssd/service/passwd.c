/* passwd.c: Common utilities for handling username transactions.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/service/passwd.h>

nssd_boolean_t nssd_service_passwd_request_name_get(const nssd_service_request_t *request,
                                                    char **name) {
  nssd_protocol_packet_field_t *field = NULL;

  NSSD_RETURN_FALSE_UNLESS(request);
  NSSD_RETURN_FALSE_UNLESS(name);
  NSSD_RETURN_FALSE_UNLESS(*name == NULL);

  nssd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != NSSD_SERVICE_PASSWD_REQUEST_NAME) {
    NSSD_LOG_ERROR("nssd_service_passwd_request_name_get: Invalid packet type");
    return NSSD_FALSE;
  }

  *name = malloc(field->size);
  if(*name == NULL) {
    NSSD_LOG_ERROR("nssd_service_passwd_request_name_get: Unable to allocate memory for name");
    return NSSD_FALSE;
  }

  strncpy(*name, field->data, field->size);
  (*name)[field->size - 1] = '\0';

  return NSSD_TRUE;
}

nssd_boolean_t nssd_service_passwd_request_name_free(const nssd_service_request_t *request,
                                                     char **name) {
  NSSD_RETURN_FALSE_UNLESS(request);
  NSSD_RETURN_FALSE_UNLESS(name);
  NSSD_RETURN_FALSE_UNLESS(*name);

  free(*name);

  return NSSD_TRUE;
}

nssd_boolean_t nssd_service_passwd_request_uid_get(const nssd_service_request_t *request,
                                                   uid_t *uid) {
  nssd_protocol_packet_field_t *field = NULL;

  NSSD_RETURN_FALSE_UNLESS(request);
  NSSD_RETURN_FALSE_UNLESS(uid);

  nssd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != NSSD_SERVICE_PASSWD_REQUEST_UID) {
    NSSD_LOG_ERROR("nssd_service_passwd_request_uid_get: Invalid packet type");
    return NSSD_FALSE;
  }

  return nssd_protocol_packet_field_get_uint32(field, (uint32_t *)uid);
}

nssd_boolean_t nssd_service_passwd_request_entities_get(const nssd_service_request_t *request,
                                                        int32_t *offset) {
  nssd_protocol_packet_field_t *field = NULL;

  NSSD_RETURN_FALSE_UNLESS(request);
  NSSD_RETURN_FALSE_UNLESS(offset);

  nssd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != NSSD_SERVICE_PASSWD_REQUEST_ENTITIES) {
    NSSD_LOG_ERROR("nssd_service_passwd_request_entites_get: Invalid packet type");
    return NSSD_FALSE;
  }

  return nssd_protocol_packet_field_get_uint32(field, (uint32_t *)offset);
}

nssd_boolean_t nssd_service_passwd_response_name_set(nssd_service_response_t *response,
                                                     const char *name, size_t length) {
  NSSD_RETURN_FALSE_UNLESS(response);
  NSSD_RETURN_FALSE_UNLESS(name);

  return nssd_protocol_packet_field_add(&response->packet, NSSD_SERVICE_PASSWD_RESPONSE_NAME,
                                        length, name);
}

nssd_boolean_t nssd_service_passwd_response_uid_set(nssd_service_response_t *response,
                                                    uid_t uid) {
  NSSD_RETURN_FALSE_UNLESS(response);

  return nssd_protocol_packet_field_add_uint32(&response->packet,
                                               NSSD_SERVICE_PASSWD_RESPONSE_UID, (uint32_t)uid);
}

nssd_boolean_t nssd_service_passwd_response_gid_set(nssd_service_response_t *response,
                                                    gid_t gid) {
  NSSD_RETURN_FALSE_UNLESS(response);

  return nssd_protocol_packet_field_add_uint32(&response->packet,
                                               NSSD_SERVICE_PASSWD_RESPONSE_GID, (uint32_t)gid);
}

nssd_boolean_t nssd_service_passwd_response_dir_set(nssd_service_response_t *response,
                                                    const char *dir, size_t length) {
  NSSD_RETURN_FALSE_UNLESS(response);
  NSSD_RETURN_FALSE_UNLESS(dir);

  return nssd_protocol_packet_field_add(&response->packet, NSSD_SERVICE_PASSWD_RESPONSE_DIR,
                                        length, dir);
}

nssd_boolean_t nssd_service_passwd_response_shell_set(nssd_service_response_t *response,
                                                      const char *shell, size_t length) {
  NSSD_RETURN_FALSE_UNLESS(response);
  NSSD_RETURN_FALSE_UNLESS(shell);

  return nssd_protocol_packet_field_add(&response->packet, NSSD_SERVICE_PASSWD_RESPONSE_SHELL,
                                        length, shell);
}
