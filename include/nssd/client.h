/* client.h: Client connector.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_CLIENT_H
#define _NSSD_CLIENT_H

/* Our client communication protocol is very simple. Each send and received
 * packet header has a predefined length (see <nssd/protocol/packet.h>); we use
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
 * procedures that are part of <nssd/protocol/serial_packet.h>. A client could
 * quite easily just take a packet and do something else with it instead of
 * serialize it. We only care that we get a packet that we can deal with
 * back. */

/* The API to access clients is very generic due to the execution flow
 * described above, with the exception that the allocation and initialization
 * functions must be called explicitly. The initializer takes client-specific
 * arguments. */

#include <nssd/nssd.h>
#include <nssd/conf.h>
#include <nssd/protocol/packet.h>

typedef struct nssd_client nssd_client_t;

typedef void (*nssd_client_free_pt)(nssd_client_t *);
typedef nssd_boolean_t (*nssd_client_finalize_pt)(nssd_client_t *);
typedef nssd_boolean_t (*nssd_client_connect_pt)(nssd_client_t *);
typedef nssd_boolean_t (*nssd_client_disconnect_pt)(nssd_client_t *);
typedef nssd_boolean_t (*nssd_client_write_pt)(nssd_client_t *, const nssd_protocol_packet_t *);
typedef nssd_boolean_t (*nssd_client_read_pt)(nssd_client_t *, nssd_protocol_packet_t *);

#define NSSD_CLIENT_HEADER                 \
  nssd_client_free_pt free;                \
  nssd_client_finalize_pt finalize;        \
  nssd_client_connect_pt connect;          \
  nssd_client_disconnect_pt disconnect;    \
  nssd_client_write_pt write;              \
  nssd_client_read_pt read;                \
  nssd_boolean_t connected

struct nssd_client {
  NSSD_CLIENT_HEADER;
};

nssd_boolean_t nssd_client_get_by_conf(nssd_client_t **, const nssd_conf_t *);

static inline void nssd_client_free(nssd_client_t *client) {
  client->free(client);
}

static inline nssd_boolean_t nssd_client_finalize(nssd_client_t *client) {
  return client->finalize(client);
}

static inline nssd_boolean_t nssd_client_connect(nssd_client_t *client) {
  return client->connect(client);
}

static inline nssd_boolean_t nssd_client_disconnect(nssd_client_t *client) {
  return client->disconnect(client);
}

static inline nssd_boolean_t nssd_client_write(nssd_client_t *client,
                                               const nssd_protocol_packet_t *packet) {
  return client->write(client, packet);
}

static inline nssd_boolean_t nssd_client_read(nssd_client_t *client,
                                              nssd_protocol_packet_t *packet) {
  return client->read(client, packet);
}

#endif
