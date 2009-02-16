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
  NSSD_RETURN_NULL_UNLESS(client);

  client->free = nssd_client_unsock_free;
  client->finalize = nssd_client_unsock_finalize;
  client->connect = nssd_client_unsock_connect;
  client->disconnect = nssd_client_unsock_disconnect;
  client->write = nssd_client_unsock_write;
  client->read = nssd_client_unsock_read;

  return client;
}

void nssd_client_unsock_free(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;

  NSSD_RETURN_UNLESS(client);

  free(client);
}

nssd_boolean_t nssd_client_unsock_initialize(nssd_client_t *cl, const char *path) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;

  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_UNLESS(path);

  client->connected = NSSD_FALSE;

  /* Set up the address. */
  client->endpoint.sun_family = AF_UNIX;
  strncpy(client->endpoint.sun_path, path, _SUN_PATH_LENGTH);

  /* Set up the socket. */
  client->socket = socket(PF_UNIX, SOCK_STREAM, 0);
  if(client->socket == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_initialize: Error creating socket");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_client_unsock_finalize(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;

  NSSD_RETURN_FALSE_UNLESS(client);

  if(client->connected) {
    if(!nssd_client_unsock_disconnect(cl)) {
      NSSD_LOG_ERROR("nssd_client_unsock_finalize: Could not disconnect "
                     "(connection terminated unexpectedly?)");
      return NSSD_FALSE;
    }
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_client_unsock_connect(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;

  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_IF(client->connected);

  if(connect(client->socket, (struct sockaddr *)&client->endpoint, sizeof(struct sockaddr_un)) == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_connect: Error opening socket");
    return NSSD_FALSE;
  }

  client->connected = NSSD_TRUE;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_client_unsock_disconnect(nssd_client_t *cl) {
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;

  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_UNLESS(client->connected);

  if(close(client->socket) == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_disconnect: Error closing socket");
    return NSSD_FALSE;
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_client_unsock_write(nssd_client_t *cl, const nssd_protocol_packet_t *packet) {
  nssd_boolean_t status = NSSD_FALSE;
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  ssize_t result;

  nssd_protocol_serial_packet_t serial;

  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_UNLESS(client->connected);
  NSSD_RETURN_FALSE_UNLESS(packet);

  nssd_protocol_serial_packet_initialize(&serial);
  if(!nssd_protocol_serial_packet_serialize(&serial, packet)) {
    NSSD_LOG_ERROR("nssd_client_unsock_write: Unable to serialize packet");
    goto _nssd_client_unsock_write_error;
  }

  /* If we ever decide to use a non-blocking implementation instead of a
   * blocking one here, we need to account for partial and deferred writes. */
  result = write(client->socket, serial.data, serial.size);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_write: Error writing data");
    goto _nssd_client_unsock_write_error;
  }
  else if((size_t)result != serial.size) {
    NSSD_LOG_ERROR("nssd_client_unsock_write: Interrupted: Wrote %d of %u bytes",
                   result, serial.size);
    goto _nssd_client_unsock_write_error;
  }

  status = NSSD_TRUE;

 _nssd_client_unsock_write_error:

  nssd_protocol_serial_packet_finalize(&serial);

  return status;
}

nssd_boolean_t nssd_client_unsock_read(nssd_client_t *cl, nssd_protocol_packet_t *packet) {
  nssd_boolean_t status = NSSD_FALSE;
  nssd_client_unsock_t *client = (nssd_client_unsock_t *)cl;
  ssize_t result;

  nssd_protocol_serial_packet_t serial;

  char header[NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE];
  char *body = NULL;
  
  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_UNLESS(client->connected);
  NSSD_RETURN_FALSE_UNLESS(packet);

  nssd_protocol_serial_packet_initialize(&serial);

  result = read(client->socket, &header, NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Error reading packet header");
    goto _nssd_client_unsock_read_error;
  }
  else if((size_t)result != NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Interrupted: Read %d of %u bytes",
                   result, NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE);
    goto _nssd_client_unsock_read_error;
  }

  if(!nssd_protocol_serial_packet_unserialize_header(&serial, packet, header)) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Error unserializing packet header");
    goto _nssd_client_unsock_read_error;
  }

  body = malloc(serial.body_size);
  if(body == NULL) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Could not allocate packet body buffer "
                   "(out of memory?)");
    goto _nssd_client_unsock_read_error;
  }

  result = read(client->socket, body, serial.body_size);
  if(result == -1) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Error reading packet body");
    goto _nssd_client_unsock_read_error;
  }
  else if((size_t)result != serial.body_size) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Interrupted: Read %d of %u bytes",
                   result, serial.body_size);
    goto _nssd_client_unsock_read_error;
  }

  if(!nssd_protocol_serial_packet_unserialize_body(&serial, packet, body)) {
    NSSD_LOG_ERROR("nssd_client_unsock_read: Error unserializing packet body");
    goto _nssd_client_unsock_read_error;
  }

  status = NSSD_TRUE;

 _nssd_client_unsock_read_error:

  nssd_protocol_serial_packet_finalize(&serial);
  if(body)
    free(body);

  return status;
}
