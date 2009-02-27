/* client.h: Client connector.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_CLIENT_H
#define _SRVD_CLIENT_H

/* Our client communication protocol is very simple. Each send and received
 * packet header has a predefined length (see <srvd/protocol/packet.h>); we use
 * this to determine how much more data we have to read from a socket after the
 * fixed-length header is read for each packet. We technically could keep
 * connections open long after each request is made and use some basic locking
 * to make everything work together nicely, but considering how lightweight
 * UNIX domain sockets are (which is what almost all users are going to be
 * using), we're not going to worry about this for now.
 *
 * To make that a bit clearer, the execution flow for a client is basically:
 *  connect -> send request -> receive response -> disconnect
 *
 * We ship two implementations, one that uses UNIX domain sockets and one that
 * uses TCP. UNIX domain sockets are highly recommended. */

/* Strictly speaking, this packet format is only enforced by the serialization
 * procedures that are part of <srvd/protocol/serial_packet.h>. A client could
 * quite easily just take a packet and do something else with it instead of
 * serialize it. We only care that we get a packet that we can deal with
 * back. */

/* The API to access clients is very generic due to the execution flow
 * described above, with the exception that the allocation and initialization
 * functions must be called explicitly. The initializer takes client-specific
 * arguments. */

#include <srvd/srvd.h>
#include <srvd/conf.h>
#include <srvd/protocol/packet.h>

typedef struct srvd_client srvd_client_t;

typedef void (*srvd_client_free_pt)(srvd_client_t *);
typedef srvd_boolean_t (*srvd_client_finalize_pt)(srvd_client_t *);
typedef srvd_boolean_t (*srvd_client_connect_pt)(srvd_client_t *);
typedef srvd_boolean_t (*srvd_client_disconnect_pt)(srvd_client_t *);
typedef srvd_boolean_t (*srvd_client_write_pt)(srvd_client_t *, const srvd_protocol_packet_t *);
typedef srvd_boolean_t (*srvd_client_read_pt)(srvd_client_t *, srvd_protocol_packet_t *);

#define SRVD_CLIENT_HEADER                 \
  srvd_client_free_pt free;                \
  srvd_client_finalize_pt finalize;        \
  srvd_client_connect_pt connect;          \
  srvd_client_disconnect_pt disconnect;    \
  srvd_client_write_pt write;              \
  srvd_client_read_pt read;                \
  srvd_boolean_t connected

struct srvd_client {
  SRVD_CLIENT_HEADER;
};

srvd_boolean_t srvd_client_get_by_conf(srvd_client_t **, const srvd_conf_t *);

static inline void srvd_client_free(srvd_client_t *client) {
  client->free(client);
}

static inline srvd_boolean_t srvd_client_finalize(srvd_client_t *client) {
  return client->finalize(client);
}

static inline srvd_boolean_t srvd_client_connect(srvd_client_t *client) {
  return client->connect(client);
}

static inline srvd_boolean_t srvd_client_disconnect(srvd_client_t *client) {
  return client->disconnect(client);
}

static inline srvd_boolean_t srvd_client_write(srvd_client_t *client,
                                               const srvd_protocol_packet_t *packet) {
  return client->write(client, packet);
}

static inline srvd_boolean_t srvd_client_read(srvd_client_t *client,
                                              srvd_protocol_packet_t *packet) {
  return client->read(client, packet);
}

#endif
