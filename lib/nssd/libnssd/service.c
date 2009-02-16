/* service.c: Support for services.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/service.h>
#include <nssd/conf.h>
#include <nssd/client.h>

nssd_service_request_t *nssd_service_request_allocate(void) {
  nssd_service_request_t *service = (nssd_service_request_t *)malloc(sizeof(nssd_service_request_t));
  if(service == NULL)
    NSSD_LOG_ERROR("nssd_service_request_allocate: Unable to allocate memory");

  return service;
}

void nssd_service_request_free(nssd_service_request_t *request) {
  NSSD_RETURN_UNLESS(request);

  free(request);
}

nssd_boolean_t nssd_service_request_initialize(nssd_service_request_t *request) {
  NSSD_RETURN_FALSE_UNLESS(request);

  if(!nssd_protocol_packet_initialize(&request->packet)) {
    NSSD_LOG_ERROR("nssd_service_request_initialize: Unable to initialize internal packet");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_service_request_finalize(nssd_service_request_t *request) {
  NSSD_RETURN_FALSE_UNLESS(request);

  if(!nssd_protocol_packet_finalize(&request->packet)) {
    NSSD_LOG_ERROR("nssd_service_request_finalize: Unable to finalize internal packet");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_service_request_query(const nssd_service_request_t *request,
                                          nssd_service_response_t *response) {
  nssd_boolean_t status = NSSD_FALSE;
  nssd_conf_file_t *fconf = NULL;
  nssd_client_t *client = NULL;
  nssd_protocol_packet_field_t *status_field = NULL;

  NSSD_RETURN_FALSE_UNLESS(request);
  NSSD_RETURN_FALSE_UNLESS(response);

  /* Read the default configuration. */
  if(!nssd_conf_file_default_get(&fconf)) {
    NSSD_LOG_ERROR("nssd_service_request_query: Unable to read configuration file "
                   "\"" NSSD_CONF_FILE_DEFAULT_PATH "\"");
    goto _nssd_service_request_query_error;
  }

  if(!nssd_client_get_by_conf(&client, &fconf->conf)) {
    NSSD_LOG_ERROR("nssd_service_request_query: Unable to create client instance");
    goto _nssd_service_request_query_error;
  }

  if(!nssd_client_connect(client)) {
    NSSD_LOG_ERROR("nssd_service_request_query: Unable to connect to remote server");
    goto _nssd_service_request_query_error;
  }

  if(!nssd_client_write(client, &request->packet)) {
    NSSD_LOG_ERROR("nssd_service_request_query: Error writing packet to server");
    goto _nssd_service_request_query_error;
  }

  if(!nssd_client_read(client, &response->packet)) {
    NSSD_LOG_ERROR("nssd_service_request_query: Error reading response packet from server");
    goto _nssd_service_request_query_error;
  }

  if(!nssd_client_disconnect(client)) {
    NSSD_LOG_WARNING("nssd_service_request_query: Unable to cleanly disconnect from server "
                     "(unexpected connection termination?)");
  }

  status = NSSD_TRUE;

 _nssd_service_request_query_error:

  /* Assuming response->packet is initialized, the field_count is either updated
   * by the query or is 0 (its initialization value), so this comparison is
   * in fact safe even if an error occurred. */
  if(!nssd_protocol_packet_field_get_first(&response->packet, &status_field)) {
    response->status = NSSD_SERVICE_RESPONSE_FAIL;
  }
  else {
    nssd_protocol_packet_field_get_uint16(status_field, &response->status);
  }

  if(client)
    nssd_client_free(client);

  return status;
}

nssd_service_response_t *nssd_service_response_allocate(void) {
  nssd_service_response_t *response = malloc(sizeof(nssd_service_response_t));
  NSSD_RETURN_NULL_UNLESS(response);

  return response;
}

void nssd_service_response_free(nssd_service_response_t *response) {
  NSSD_RETURN_UNLESS(response);

  free(response);
}

nssd_boolean_t nssd_service_response_initialize(nssd_service_response_t *response) {
  NSSD_RETURN_FALSE_UNLESS(response);

  response->status = NSSD_SERVICE_RESPONSE_UNKNOWN;

  if(!nssd_protocol_packet_initialize(&response->packet)) {
    NSSD_LOG_ERROR("nssd_service_response_initialize: Unable to initialize internal packet");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_service_response_finalize(nssd_service_response_t *response) {
  NSSD_RETURN_FALSE_UNLESS(response);

  response->status = NSSD_SERVICE_RESPONSE_UNKNOWN;

  if(!nssd_protocol_packet_finalize(&response->packet)) {
    NSSD_LOG_ERROR("nssd_service_response_finalize: Unable to finalize internal packet");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}
