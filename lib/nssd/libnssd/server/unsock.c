/* unsock.c: UNIX socket server.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/server/unsock.h>
#include <nssd/protocol/serial_packet.h>

#include <stdio.h>

/* The POSIX standard defines no recommended length for sun_path, so we
 * determine it here based on whatever the system actually uses. */
#define _SUN_PATH_LENGTH \
  ((size_t)(sizeof(((struct sockaddr_un *)NULL)->sun_path) / sizeof(char)))

nssd_server_unsock_t *nssd_server_unsock_allocate(void) {
  nssd_server_unsock_t *server = malloc(sizeof(nssd_server_unsock_t));
  NSSD_RETURN_NULL_UNLESS(server);

  return server;
}

void nssd_server_unsock_free(nssd_server_unsock_t *server) {
  NSSD_RETURN_UNLESS(server);

  free(server);
}

nssd_boolean_t nssd_server_unsock_initialize(nssd_server_unsock_t *server,
                                             const nssd_server_unsock_conf_t *conf) {
  NSSD_RETURN_FALSE_UNLESS(server);
  NSSD_RETURN_FALSE_UNLESS(conf);
  NSSD_RETURN_FALSE_UNLESS(conf->path);

  if(!nssd_server_initialize(&server->monitor)) {
    NSSD_LOG_ERROR("nssd_server_unsock_initialize: Unable to initialize server monitor");
    return NSSD_FALSE;
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
    NSSD_LOG_ERROR("nssd_server_unsock_initialize: Error creating socket");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_server_unsock_finalize(nssd_server_unsock_t *server) {
  NSSD_RETURN_FALSE_UNLESS(server);

  if(!server->monitor.executing) {
    NSSD_LOG_ERROR("nssd_server_unsock_finalize: Cannot finalize: Server is still executing");
    return NSSD_FALSE;
  }

  if(!nssd_server_finalize(&server->monitor)) {
    NSSD_LOG_ERROR("nssd_server_unsock_finalize: Unable to finalize server monitor");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

static nssd_boolean_t _nssd_server_unsock_read(int from, nssd_protocol_packet_t *packet) {
  nssd_boolean_t status = NSSD_FALSE;
  ssize_t result;

  nssd_protocol_serial_packet_t serial;

  char header[NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE];
  char *body = NULL;

  nssd_protocol_serial_packet_initialize(&serial);

  result = read(from, &header, NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Error reading packet header");
    goto __nssd_server_unsock_read_error;
  }
  else if((size_t)result != NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Interrupted: Read %d of %u bytes",
                   result, NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
    goto __nssd_server_unsock_read_error;
  }

  if(!nssd_protocol_serial_packet_unserialize_header(&serial, packet, header)) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Error unserializing packet header");
    goto __nssd_server_unsock_read_error;
  }

  body = malloc(serial.body_size);
  if(body == NULL) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Could not allocate packet body buffer "
                   "(out of memory?)");
    goto __nssd_server_unsock_read_error;
  }

  result = read(from, body, serial.body_size);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Error reading packet body");
    goto __nssd_server_unsock_read_error;
  }
  else if((size_t)result != serial.body_size) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Interrupted: Read %d of %u bytes",
                   result, serial.body_size);
    goto __nssd_server_unsock_read_error;
  }

  if(!nssd_protocol_serial_packet_unserialize_body(&serial, packet, body)) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Error unserializing packet body");
    goto __nssd_server_unsock_read_error;
  }

  status = NSSD_TRUE;

 __nssd_server_unsock_read_error:

  nssd_protocol_serial_packet_finalize(&serial);
  if(body)
    free(body);

  return status;
}

static nssd_boolean_t _nssd_server_unsock_write(int to, const nssd_protocol_packet_t *packet) {
  nssd_boolean_t status = NSSD_FALSE;
  ssize_t result;

  nssd_protocol_serial_packet_t serial;

  nssd_protocol_serial_packet_initialize(&serial);
  if(!nssd_protocol_serial_packet_serialize(&serial, packet)) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Unable to serialize packet");
    goto __nssd_server_unsock_write_error;
  }

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  result = write(to, serial.data, serial.size);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Error writing data");
    goto __nssd_server_unsock_write_error;
  }
  else if((size_t)result != serial.size) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Interrupted: Wrote %d of %u bytes",
                   result, serial.size);
    goto __nssd_server_unsock_write_error;
  }

  status = NSSD_TRUE;

 __nssd_server_unsock_write_error:

  nssd_protocol_serial_packet_finalize(&serial);

  return status;
}

nssd_boolean_t nssd_server_unsock_execute(nssd_server_unsock_t *server) {
  nssd_boolean_t status = NSSD_TRUE;

  NSSD_RETURN_FALSE_UNLESS(server);

  server->monitor.executing = NSSD_TRUE;
  if(bind(server->socket, (struct sockaddr *)&server->endpoint, sizeof(struct sockaddr_un)) == -1) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Unable to bind to socket at \"%s\"",
                   server->endpoint.sun_path);
    return NSSD_FALSE;
  }
  if(listen(server->socket, server->conf.queue_size) == -1) {
    NSSD_LOG_ERROR("nssd_server_unsock_execute: Unable to listen on bound socket");
    return NSSD_FALSE;
  }

  for(;;) {
    int client = accept(server->socket, NULL, 0);
    if(client == -1) {
      NSSD_LOG_WARNING("nssd_server_unsock_execute: Error accept()ing client");
      continue;
    }

    nssd_service_request_t request;
    nssd_service_response_t response;
    nssd_service_request_initialize(&request);
    nssd_service_response_initialize(&response);

    if(!_nssd_server_unsock_read(client, &request.packet)) {
      NSSD_LOG_WARNING("nssd_server_unsock_execute: Could not read data from client");
      goto _nssd_server_unsock_execute_client_error;
    }

    nssd_protocol_packet_field_t *field = NULL;
    if(!nssd_protocol_packet_field_get_first(&request.packet, &field)) {
      /* Nothing valid? */
      NSSD_LOG_WARNING("nssd_server_unsock_execute: Invalid packet");
      goto _nssd_server_unsock_execute_client_error;
    }

    /* Okay, let's see if we have a matching handler for the request. */
    nssd_server_service_handler_pt handler = NULL;
    if(nssd_server_service_get(&server->monitor, field->type, &handler)) {
      handler(&request, &response);

      /* Get the response status and inject it into the list of fields. */
      nssd_protocol_packet_field_inject_uint16(&response.packet, NSSD_PROTOCOL_STATUS,
                                               response.status);
    }
    else
      /* Nope -- unavailable. */
      nssd_protocol_packet_field_inject_uint16(&response.packet, NSSD_PROTOCOL_STATUS,
                                               NSSD_SERVICE_RESPONSE_UNAVAIL);

    if(!_nssd_server_unsock_write(client, &response.packet)) {
      NSSD_LOG_WARNING("nssd_server_unsock_execute: Could not write data to client");
      goto _nssd_server_unsock_execute_client_error;
    }

  _nssd_server_unsock_execute_client_error:

    nssd_service_request_finalize(&request);
    nssd_service_response_finalize(&response);

    if(client >= 0)
      close(client);
  }

  close(server->socket);
  server->monitor.executing = NSSD_FALSE;

  return status;
}
