/* unsock.c: UNIX socket server.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/server/unsock.h>
#include <srvd/protocol/serial_packet.h>

#include <stdio.h>

/* The POSIX standard defines no recommended length for sun_path, so we
 * determine it here based on whatever the system actually uses. */
#define _SUN_PATH_LENGTH \
  ((size_t)(sizeof(((struct sockaddr_un *)NULL)->sun_path) / sizeof(char)))

srvd_server_unsock_t *srvd_server_unsock_allocate(void) {
  srvd_server_unsock_t *server = malloc(sizeof(srvd_server_unsock_t));
  SRVD_RETURN_NULL_UNLESS(server);

  return server;
}

void srvd_server_unsock_free(srvd_server_unsock_t *server) {
  SRVD_RETURN_UNLESS(server);

  free(server);
}

srvd_boolean_t srvd_server_unsock_initialize(srvd_server_unsock_t *server,
                                             const srvd_server_unsock_conf_t *conf) {
  SRVD_RETURN_FALSE_UNLESS(server);
  SRVD_RETURN_FALSE_UNLESS(conf);
  SRVD_RETURN_FALSE_UNLESS(conf->path);

  if(!srvd_server_initialize(&server->monitor)) {
    SRVD_LOG_ERROR("srvd_server_unsock_initialize: Unable to initialize server monitor");
    return SRVD_FALSE;
  }

  /* Copy the configuration. */
  server->conf = *conf;

  /* Set up the address. */
  server->endpoint.sun_family = AF_UNIX;
  strncpy(server->endpoint.sun_path, server->conf.path, _SUN_PATH_LENGTH);
  server->endpoint.sun_path[_SUN_PATH_LENGTH - 1] = '\0';

  /* Set up the socket. */
  server->socket = socket(PF_UNIX, SOCK_STREAM, 0);
  if(server->socket == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_initialize: Error creating socket");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_server_unsock_finalize(srvd_server_unsock_t *server) {
  SRVD_RETURN_FALSE_UNLESS(server);

  if(!server->monitor.executing) {
    SRVD_LOG_ERROR("srvd_server_unsock_finalize: Cannot finalize: Server is still executing");
    return SRVD_FALSE;
  }

  if(!srvd_server_finalize(&server->monitor)) {
    SRVD_LOG_ERROR("srvd_server_unsock_finalize: Unable to finalize server monitor");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

static srvd_boolean_t _srvd_server_unsock_read(int from, srvd_protocol_packet_t *packet) {
  srvd_boolean_t status = SRVD_FALSE;
  ssize_t result;

  srvd_protocol_serial_packet_t serial;

  char header[SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE];
  char *body = NULL;

  srvd_protocol_serial_packet_initialize(&serial);

  result = read(from, &header, SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Error reading packet header");
    goto __srvd_server_unsock_read_error;
  }
  else if((size_t)result != SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Interrupted: Read %d of %u bytes",
                   result, SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
    goto __srvd_server_unsock_read_error;
  }

  if(!srvd_protocol_serial_packet_unserialize_header(&serial, packet, header)) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Error unserializing packet header");
    goto __srvd_server_unsock_read_error;
  }

  body = malloc(serial.body_size);
  if(body == NULL) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Could not allocate packet body buffer "
                   "(out of memory?)");
    goto __srvd_server_unsock_read_error;
  }

  result = read(from, body, serial.body_size);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Error reading packet body");
    goto __srvd_server_unsock_read_error;
  }
  else if((size_t)result != serial.body_size) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Interrupted: Read %d of %u bytes",
                   result, serial.body_size);
    goto __srvd_server_unsock_read_error;
  }

  if(!srvd_protocol_serial_packet_unserialize_body(&serial, packet, body)) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Error unserializing packet body");
    goto __srvd_server_unsock_read_error;
  }

  status = SRVD_TRUE;

 __srvd_server_unsock_read_error:

  srvd_protocol_serial_packet_finalize(&serial);
  if(body)
    free(body);

  return status;
}

static srvd_boolean_t _srvd_server_unsock_write(int to, const srvd_protocol_packet_t *packet) {
  srvd_boolean_t status = SRVD_FALSE;
  ssize_t result;

  srvd_protocol_serial_packet_t serial;

  srvd_protocol_serial_packet_initialize(&serial);
  if(!srvd_protocol_serial_packet_serialize(&serial, packet)) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Unable to serialize packet");
    goto __srvd_server_unsock_write_error;
  }

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  result = write(to, serial.data, serial.size);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Error writing data");
    goto __srvd_server_unsock_write_error;
  }
  else if((size_t)result != serial.size) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Interrupted: Wrote %d of %u bytes",
                   result, serial.size);
    goto __srvd_server_unsock_write_error;
  }

  status = SRVD_TRUE;

 __srvd_server_unsock_write_error:

  srvd_protocol_serial_packet_finalize(&serial);

  return status;
}

srvd_boolean_t srvd_server_unsock_execute(srvd_server_unsock_t *server) {
  srvd_boolean_t status = SRVD_TRUE;

  SRVD_RETURN_FALSE_UNLESS(server);

  server->monitor.executing = SRVD_TRUE;
  if(bind(server->socket, (struct sockaddr *)&server->endpoint, sizeof(struct sockaddr_un)) == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Unable to bind to socket at \"%s\"",
                   server->endpoint.sun_path);
    return SRVD_FALSE;
  }
  if(listen(server->socket, server->conf.queue_size) == -1) {
    SRVD_LOG_ERROR("srvd_server_unsock_execute: Unable to listen on bound socket");
    return SRVD_FALSE;
  }

  for(;;) {
    int client = accept(server->socket, NULL, 0);
    if(client == -1) {
      SRVD_LOG_WARNING("srvd_server_unsock_execute: Error accept()ing client");
      continue;
    }

    srvd_service_request_t request;
    srvd_service_response_t response;
    srvd_service_request_initialize(&request);
    srvd_service_response_initialize(&response);

    if(!_srvd_server_unsock_read(client, &request.packet)) {
      SRVD_LOG_WARNING("srvd_server_unsock_execute: Could not read data from client");
      goto _srvd_server_unsock_execute_client_error;
    }

    srvd_protocol_packet_field_t *field = NULL;
    if(!srvd_protocol_packet_field_get_first(&request.packet, &field)) {
      /* Nothing valid? */
      SRVD_LOG_WARNING("srvd_server_unsock_execute: Invalid packet");
      goto _srvd_server_unsock_execute_client_error;
    }

    /* Okay, let's see if we have a matching handler for the request. */
    srvd_server_service_handler_pt handler = NULL;
    if(srvd_server_service_get(&server->monitor, field->type, &handler)) {
      handler(&request, &response);

      /* Get the response status and inject it into the list of fields. */
      srvd_protocol_packet_field_inject_uint16(&response.packet, SRVD_PROTOCOL_STATUS,
                                               response.status);
    }
    else
      /* Nope -- unavailable. */
      srvd_protocol_packet_field_inject_uint16(&response.packet, SRVD_PROTOCOL_STATUS,
                                               SRVD_SERVICE_RESPONSE_UNAVAIL);

    if(!_srvd_server_unsock_write(client, &response.packet)) {
      SRVD_LOG_WARNING("srvd_server_unsock_execute: Could not write data to client");
      goto _srvd_server_unsock_execute_client_error;
    }

  _srvd_server_unsock_execute_client_error:

    srvd_service_request_finalize(&request);
    srvd_service_response_finalize(&response);

    if(client >= 0)
      close(client);
  }

  close(server->socket);
  server->monitor.executing = SRVD_FALSE;

  return status;
}
