/* aliases.c: Support for the aliases database.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/service/nss/aliases.h>

srvd_boolean_t srvd_service_nss_aliases_request_name_get(const srvd_service_request_t *request,
                                                         char **name) {
  srvd_protocol_packet_field_t *field = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(name);
  SRVD_RETURN_FALSE_UNLESS(*name == NULL);

  srvd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != SRVD_SERVICE_NSS_ALIASES_REQUEST_NAME) {
    SRVD_LOG_ERROR("srvd_service_nss_aliases_request_name_get: Invalid packet type");
    return SRVD_FALSE;
  }

  *name = malloc(field->size);
  if(*name == NULL) {
    SRVD_LOG_ERROR("srvd_service_nss_aliases_request_name_get: Unable to allocate memory for name");
    return SRVD_FALSE;
  }

  strncpy(*name, field->data, field->size);
  (*name)[field->size - 1] = '\0';

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_nss_aliases_request_name_free(const srvd_service_request_t *request,
                                                          char **name) {
  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(name);
  SRVD_RETURN_FALSE_UNLESS(*name);

  free(*name);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_nss_aliases_request_entities_get(const srvd_service_request_t *request,
                                                             int32_t *offset) {
  srvd_protocol_packet_field_t *field = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(offset);

  srvd_protocol_packet_field_get_first(&request->packet, &field);
  if(field->type != SRVD_SERVICE_NSS_ALIASES_REQUEST_ENTITIES) {
    SRVD_LOG_ERROR("srvd_service_nss_aliases_request_entites_get: Invalid packet type");
    return SRVD_FALSE;
  }

  return srvd_protocol_packet_field_get_uint32(field, (uint32_t *)offset);
}

srvd_boolean_t srvd_service_nss_aliases_response_name_set(srvd_service_response_t *response,
                                                          const char *name, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(name);

  return srvd_protocol_packet_field_add(&response->packet, SRVD_SERVICE_NSS_ALIASES_RESPONSE_NAME,
                                        length, name);
}

srvd_boolean_t srvd_service_nss_aliases_response_member_add(srvd_service_response_t *response,
                                                            const char *member, size_t length) {
  SRVD_RETURN_FALSE_UNLESS(response);
  SRVD_RETURN_FALSE_UNLESS(member);

  return srvd_protocol_packet_field_add(&response->packet, SRVD_SERVICE_NSS_ALIASES_RESPONSE_MEMBER,
                                        length, member);
}

srvd_boolean_t srvd_service_nss_aliases_response_local_set(srvd_service_response_t *response,
                                                           srvd_boolean_t local) {
  SRVD_RETURN_FALSE_UNLESS(response);

  return srvd_protocol_packet_field_add_uint8(&response->packet,
                                              SRVD_SERVICE_NSS_ALIASES_RESPONSE_LOCAL, (uint8_t)local);
}
