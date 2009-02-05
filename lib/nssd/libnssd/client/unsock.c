/* unsock.c: UNIX socket client.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/client.h>
#include <nssd/client/unsock.h>

#include <nssd/protocol/serial_packet.h>

/* The POSIX standard defines no recommended length for sun_path, so we
 * determine it here based on whatever the system actually uses. */
#define _SUN_PATH_LENGTH \
  ((size_t)(sizeof(((struct sockaddr_un *)NULL)->sun_path) / sizeof(char)))

nssd_client_t *nssd_client_unsock_allocate(void) {
  nssd_client_t *client = (nssd_client_t *)malloc(sizeof(nssd_client_unsock_t));
  assert(client);

  client->allocate = nssd_client_unsock_allocate;
  client->free = nssd_client_unsock_free;
  client->initialize = nssd_client_unsock_initialize;
  client->finalize = nssd_client_unsock_finalize;
  client->connect = nssd_client_unsock_connect;
  client->disconnect = nssd_client_unsock_disconnect;
  client->write = nssd_client_unsock_write;
  client->read = nssd_client_unsock_read;

  return client;
}

void nssd_client_unsock_free(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);

  free(client);
}

void nssd_client_unsock_initialize(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);

  client->connected = 0;

  /* Set up the address. */
  client->endpoint.sun_family = AF_UNIX;
  strncpy(client->endpoint.sun_path, "/tmp/nssd-sample.sock", _SUN_PATH_LENGTH);

  /* Set up the socket. */
  client->socket = socket(PF_UNIX, SOCK_STREAM, 0);
  assert(client->socket != -1);
}

void nssd_client_unsock_finalize(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);

  if(client->connected)
    nssd_client_unsock_disconnect(cl);
}

void nssd_client_unsock_connect(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);

  assert(connect(client->socket, (struct sockaddr *)&client->endpoint, sizeof(struct sockaddr_un)) != -1);
  client->connected = 1;
}

void nssd_client_unsock_disconnect(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);
  assert(client->connected);

  close(client->socket);
}

void nssd_client_unsock_write(nssd_client_t *cl, const nssd_protocol_packet_t *packet) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);
  assert(client->connected);
  assert(packet);

  nssd_protocol_serial_packet_t serial;
  nssd_protocol_serial_packet_initialize(&serial);
  nssd_protocol_serial_packet_serialize(&serial, packet);

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  assert(write(client->socket, serial.data, serial.length) == serial.length);

  nssd_protocol_serial_packet_finalize(&serial);
}

void nssd_client_unsock_read(nssd_client_t *cl, nssd_protocol_packet_t *packet) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  assert(client);
  assert(client->connected);
  assert(packet);

  nssd_protocol_serial_packet_t serial;
  nssd_protocol_serial_packet_initialize(&serial);

  char header[NSSD_PROTOCOL_PACKET_HEADER_SIZE];
  assert(read(client->socket, &header, NSSD_PROTOCOL_PACKET_HEADER_SIZE) == NSSD_PROTOCOL_PACKET_HEADER_SIZE);
  nssd_protocol_serial_packet_unserialize_header(&serial, packet, header);

  char *body = malloc(serial.body_length);
  assert(read(client->socket, body, serial.body_length) == serial.body_length);
  nssd_protocol_serial_packet_unserialize_body(&serial, packet, body);

  nssd_protocol_serial_packet_finalize(&serial);
  free(body);
}
