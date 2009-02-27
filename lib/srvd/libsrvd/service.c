/* service.c: Support for services.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/service.h>
#include <srvd/conf.h>
#include <srvd/client.h>

srvd_service_request_t *srvd_service_request_allocate(void) {
  srvd_service_request_t *service = malloc(sizeof(srvd_service_request_t));
  if(service == NULL)
    SRVD_LOG_ERROR("srvd_service_request_allocate: Unable to allocate memory");

  return service;
}

void srvd_service_request_free(srvd_service_request_t *request) {
  SRVD_RETURN_UNLESS(request);

  free(request);
}

srvd_boolean_t srvd_service_request_initialize(srvd_service_request_t *request) {
  SRVD_RETURN_FALSE_UNLESS(request);

  if(!srvd_protocol_packet_initialize(&request->packet)) {
    SRVD_LOG_ERROR("srvd_service_request_initialize: Unable to initialize internal packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_request_finalize(srvd_service_request_t *request) {
  SRVD_RETURN_FALSE_UNLESS(request);

  if(!srvd_protocol_packet_finalize(&request->packet)) {
    SRVD_LOG_ERROR("srvd_service_request_finalize: Unable to finalize internal packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_request_query(const srvd_service_request_t *request,
                                          srvd_service_response_t *response) {
  srvd_boolean_t status = SRVD_FALSE;
  srvd_conf_file_t *fconf = NULL;
  srvd_client_t *client = NULL;
  srvd_protocol_packet_field_t *status_field = NULL;

  SRVD_RETURN_FALSE_UNLESS(request);
  SRVD_RETURN_FALSE_UNLESS(response);

  /* Read the default configuration. */
  if(!srvd_conf_file_default_get(&fconf)) {
    SRVD_LOG_ERROR("srvd_service_request_query: Unable to read configuration file "
                   "\"" SRVD_CONF_FILE_DEFAULT_PATH "\"");
    goto _srvd_service_request_query_error;
  }

  if(!srvd_client_get_by_conf(&client, &fconf->conf)) {
    SRVD_LOG_ERROR("srvd_service_request_query: Unable to create client instance");
    goto _srvd_service_request_query_error;
  }

  if(!srvd_client_connect(client)) {
    SRVD_LOG_ERROR("srvd_service_request_query: Unable to connect to remote server");
    goto _srvd_service_request_query_error;
  }

  if(!srvd_client_write(client, &request->packet)) {
    SRVD_LOG_ERROR("srvd_service_request_query: Error writing packet to server");
    goto _srvd_service_request_query_error;
  }

  if(!srvd_client_read(client, &response->packet)) {
    SRVD_LOG_ERROR("srvd_service_request_query: Error reading response packet from server");
    goto _srvd_service_request_query_error;
  }

  if(!srvd_client_disconnect(client)) {
    SRVD_LOG_WARNING("srvd_service_request_query: Unable to cleanly disconnect from server "
                     "(unexpected connection termination?)");
  }

  status = SRVD_TRUE;

 _srvd_service_request_query_error:

  /* Assuming response->packet is initialized, the field_count is either updated
   * by the query or is 0 (its initialization value), so this comparison is
   * in fact safe even if an error occurred. */
  if(!srvd_protocol_packet_field_get_first(&response->packet, &status_field)) {
    response->status = SRVD_SERVICE_RESPONSE_FAIL;
  }
  else {
    srvd_protocol_packet_field_get_uint16(status_field, &response->status);
  }

  if(client)
    srvd_client_free(client);

  return status;
}

srvd_service_response_t *srvd_service_response_allocate(void) {
  srvd_service_response_t *response = malloc(sizeof(srvd_service_response_t));
  SRVD_RETURN_NULL_UNLESS(response);

  return response;
}

void srvd_service_response_free(srvd_service_response_t *response) {
  SRVD_RETURN_UNLESS(response);

  free(response);
}

srvd_boolean_t srvd_service_response_initialize(srvd_service_response_t *response) {
  SRVD_RETURN_FALSE_UNLESS(response);

  response->status = SRVD_SERVICE_RESPONSE_UNKNOWN;

  if(!srvd_protocol_packet_initialize(&response->packet)) {
    SRVD_LOG_ERROR("srvd_service_response_initialize: Unable to initialize internal packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_service_response_finalize(srvd_service_response_t *response) {
  SRVD_RETURN_FALSE_UNLESS(response);

  response->status = SRVD_SERVICE_RESPONSE_UNKNOWN;

  if(!srvd_protocol_packet_finalize(&response->packet)) {
    SRVD_LOG_ERROR("srvd_service_response_finalize: Unable to finalize internal packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}
