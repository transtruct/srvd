/* unsock.c: UNIX socket server.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/server.h>
#include <nssd/server/unsock.h>

#include <nssd/protocol/response.h>
#include <nssd/protocol/serial_packet.h>

#include <stdio.h>

/* The POSIX standard defines no recommended length for sun_path, so we
 * determine it here based on whatever the system actually uses. */
#define _SUN_PATH_LENGTH \
  ((size_t)(sizeof(((struct sockaddr_un *)NULL)->sun_path) / sizeof(char)))

#define _CQ 4

nssd_server_t *nssd_server_unsock_allocate(void) {
  nssd_server_t *server = (nssd_server_t *)malloc(sizeof(nssd_server_unsock_t));

  server->free = nssd_server_unsock_free;
  server->initialize = nssd_server_unsock_initialize;
  server->finalize = nssd_server_unsock_finalize;
  server->execute = nssd_server_unsock_execute;

  return server;
}

void nssd_server_unsock_free(nssd_server_t *sv) {
  nssd_server_unsock_t *server = (nssd_server_unsock_t *)sv;
  assert(server);

  free(server);
}

void nssd_server_unsock_initialize(nssd_server_t *sv) {
  nssd_server_unsock_t *server = (nssd_server_unsock_t *)sv;
  assert(server);

  /* Set up the address. */
  server->endpoint.sun_family = AF_UNIX;
  strncpy(server->endpoint.sun_path, "/tmp/nssd.sock", _SUN_PATH_LENGTH);

  /* Set up the socket. */
  server->socket = socket(PF_UNIX, SOCK_STREAM, 0);
  assert(server->socket != -1);
}

void nssd_server_unsock_finalize(nssd_server_t *sv) {
  nssd_server_unsock_t *server = (nssd_server_unsock_t *)sv;
  assert(server);

  assert(!server->executing);
}

static void _nssd_server_unsock_read(int from, nssd_protocol_packet_t *packet) {
  assert(packet);

  nssd_protocol_serial_packet_t serial;
  nssd_protocol_serial_packet_initialize(&serial);

  char header[NSSD_PROTOCOL_PACKET_HEADER_SIZE];
  assert(read(from, &header, NSSD_PROTOCOL_PACKET_HEADER_SIZE) == NSSD_PROTOCOL_PACKET_HEADER_SIZE);
  nssd_protocol_serial_packet_unserialize_header(&serial, packet, header);

  char *body = malloc(serial.body_length);
  assert(read(from, body, serial.body_length) == serial.body_length);
  nssd_protocol_serial_packet_unserialize_body(&serial, packet, body);

  nssd_protocol_serial_packet_finalize(&serial);
  free(body);
}

static void _nssd_server_unsock_write(int to, const nssd_protocol_packet_t *packet) {
  assert(packet);

  nssd_protocol_serial_packet_t serial;
  nssd_protocol_serial_packet_initialize(&serial);
  nssd_protocol_serial_packet_serialize(&serial, packet);

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  assert(write(to, serial.data, serial.length) == serial.length);

  nssd_protocol_serial_packet_finalize(&serial);
}

void nssd_server_unsock_execute(nssd_server_t *sv) {
  nssd_server_unsock_t *server = (nssd_server_unsock_t *)sv;
  assert(server);

  server->executing = NSSD_TRUE;
  assert(bind(server->socket, (struct sockaddr *)&server->endpoint, sizeof(struct sockaddr_un)) != -1);
  assert(listen(server->socket, _CQ) != -1);

  for(;;) {
    int client = accept(server->socket, NULL, 0);
    assert(client != -1);

    nssd_protocol_packet_t request, response;
    nssd_protocol_packet_initialize(&request);
    nssd_protocol_packet_initialize(&response);

    _nssd_server_unsock_read(client, &request);

    assert(request.field_count > 0);

    /* Okay, let's see if we have a matching handler for the request. */
    nssd_server_service_handler_pt handler = NULL;
    if(nssd_server_service_get(server, request.fields[0].type, &handler)) {
      handler(&request, &response);
    }
    else {
      /* Nope -- unavailable. */
      nssd_protocol_packet_fields_initialize(&response, 1);

      response.fields[0].type = NSSD_PROTOCOL_RESP_STATUS;
      response.fields[0].length = sizeof(uint16_t);

      nssd_protocol_packet_field_initialize(&response, 0);
      *(uint16_t *)response.fields[0].data = htons(NSSD_PROTOCOL_RESPONSE_UNAVAIL);
    }
    
    _nssd_server_unsock_write(client, &response);

    nssd_protocol_packet_finalize(&request);
    nssd_protocol_packet_finalize(&response);

    close(client);
  }
}
