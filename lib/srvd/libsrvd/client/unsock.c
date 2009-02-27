/* unsock.c: UNIX socket client.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/client.h>
#include <srvd/client/unsock.h>

#include <srvd/protocol/serial_packet.h>

/* The POSIX standard defines no recommended length for sun_path, so we
 * determine it here based on whatever the system actually uses. */
#define _SUN_PATH_LENGTH \
  ((size_t)(sizeof(((struct sockaddr_un *)NULL)->sun_path) / sizeof(char)))

srvd_client_t *srvd_client_unsock_allocate(void) {
  srvd_client_t *client = (srvd_client_t *)malloc(sizeof(srvd_client_unsock_t));
  SRVD_RETURN_NULL_UNLESS(client);

  client->free = srvd_client_unsock_free;
  client->finalize = srvd_client_unsock_finalize;
  client->connect = srvd_client_unsock_connect;
  client->disconnect = srvd_client_unsock_disconnect;
  client->write = srvd_client_unsock_write;
  client->read = srvd_client_unsock_read;

  return client;
}

void srvd_client_unsock_free(srvd_client_t *cl) {
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;

  SRVD_RETURN_UNLESS(client);

  free(client);
}

srvd_boolean_t srvd_client_unsock_initialize(srvd_client_t *cl, const char *path) {
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;

  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_UNLESS(path);

  client->connected = SRVD_FALSE;

  /* Set up the address. */
  client->endpoint.sun_family = AF_UNIX;
  strncpy(client->endpoint.sun_path, path, _SUN_PATH_LENGTH);

  /* Set up the socket. */
  client->socket = socket(PF_UNIX, SOCK_STREAM, 0);
  if(client->socket == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_initialize: Error creating socket");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_client_unsock_finalize(srvd_client_t *cl) {
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;

  SRVD_RETURN_FALSE_UNLESS(client);

  if(client->connected) {
    if(!srvd_client_unsock_disconnect(cl)) {
      SRVD_LOG_ERROR("srvd_client_unsock_finalize: Could not disconnect "
                     "(connection terminated unexpectedly?)");
      return SRVD_FALSE;
    }
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_client_unsock_connect(srvd_client_t *cl) {
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;

  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_IF(client->connected);

  if(connect(client->socket, (struct sockaddr *)&client->endpoint, sizeof(struct sockaddr_un)) == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_connect: Error opening socket");
    return SRVD_FALSE;
  }

  client->connected = SRVD_TRUE;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_client_unsock_disconnect(srvd_client_t *cl) {
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;

  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_UNLESS(client->connected);

  if(close(client->socket) == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_disconnect: Error closing socket");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_client_unsock_write(srvd_client_t *cl, const srvd_protocol_packet_t *packet) {
  srvd_boolean_t status = SRVD_FALSE;
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;
  ssize_t result;

  srvd_protocol_serial_packet_t serial;

  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_UNLESS(client->connected);
  SRVD_RETURN_FALSE_UNLESS(packet);

  srvd_protocol_serial_packet_initialize(&serial);
  if(!srvd_protocol_serial_packet_serialize(&serial, packet)) {
    SRVD_LOG_ERROR("srvd_client_unsock_write: Unable to serialize packet");
    goto _srvd_client_unsock_write_error;
  }

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  result = write(client->socket, serial.data, serial.size);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_write: Error writing data");
    goto _srvd_client_unsock_write_error;
  }
  else if((size_t)result != serial.size) {
    SRVD_LOG_ERROR("srvd_client_unsock_write: Interrupted: Wrote %d of %u bytes",
                   result, serial.size);
    goto _srvd_client_unsock_write_error;
  }

  status = SRVD_TRUE;

 _srvd_client_unsock_write_error:

  srvd_protocol_serial_packet_finalize(&serial);

  return status;
}

srvd_boolean_t srvd_client_unsock_read(srvd_client_t *cl, srvd_protocol_packet_t *packet) {
  srvd_boolean_t status = SRVD_FALSE;
  srvd_client_unsock_t *client = (srvd_client_unsock_t *)cl;
  ssize_t result;

  srvd_protocol_serial_packet_t serial;

  char header[SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE];
  char *body = NULL;
  
  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_UNLESS(client->connected);
  SRVD_RETURN_FALSE_UNLESS(packet);

  srvd_protocol_serial_packet_initialize(&serial);

  result = read(client->socket, &header, SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Error reading packet header");
    goto _srvd_client_unsock_read_error;
  }
  else if((size_t)result != SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Interrupted: Read %d of %u bytes",
                   result, SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
    goto _srvd_client_unsock_read_error;
  }

  if(!srvd_protocol_serial_packet_unserialize_header(&serial, packet, header)) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Error unserializing packet header");
    goto _srvd_client_unsock_read_error;
  }

  body = malloc(serial.body_size);
  if(body == NULL) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Could not allocate packet body buffer "
                   "(out of memory?)");
    goto _srvd_client_unsock_read_error;
  }

  result = read(client->socket, body, serial.body_size);
  if(result == -1) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Error reading packet body");
    goto _srvd_client_unsock_read_error;
  }
  else if((size_t)result != serial.body_size) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Interrupted: Read %d of %u bytes",
                   result, serial.body_size);
    goto _srvd_client_unsock_read_error;
  }

  if(!srvd_protocol_serial_packet_unserialize_body(&serial, packet, body)) {
    SRVD_LOG_ERROR("srvd_client_unsock_read: Error unserializing packet body");
    goto _srvd_client_unsock_read_error;
  }

  status = SRVD_TRUE;

 _srvd_client_unsock_read_error:

  srvd_protocol_serial_packet_finalize(&serial);
  if(body)
    free(body);

  return status;
}
