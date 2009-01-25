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

#include <nssd/nssd.h>
#include <nssd/protocol/packet.h>

typedef struct nssd_client nssd_client_t;

typedef nssd_client_t *(*nssd_client_allocate_pt)(void);
typedef void (*nssd_client_free_pt)(nssd_client_t *);
typedef void (*nssd_client_initialize_pt)(nssd_client_t *);
typedef void (*nssd_client_finalize_pt)(nssd_client_t *);
typedef void (*nssd_client_connect_pt)(nssd_client_t *);
typedef void (*nssd_client_disconnect_pt)(nssd_client_t *);
typedef void (*nssd_client_write_pt)(nssd_client_t *, const nssd_protocol_packet_t *);
typedef void (*nssd_client_read_pt)(nssd_client_t *, nssd_protocol_packet_t *);

#define NSSD_CLIENT_HEADER                 \
  nssd_client_allocate_pt allocate;        \
  nssd_client_free_pt free;                \
  nssd_client_initialize_pt initialize;    \
  nssd_client_finalize_pt finalize;        \
  nssd_client_connect_pt connect;          \
  nssd_client_disconnect_pt disconnect;    \
  nssd_client_write_pt write;              \
  nssd_client_read_pt read;                \
  uint8_t connected

struct nssd_client {
  NSSD_CLIENT_HEADER;
};

#define NSSD_CLIENT_FREE(cl) (cl)->free((cl))
#define NSSD_CLIENT_INITIALIZE(cl) (cl)->initialize((cl))
#define NSSD_CLIENT_FINALIZE(cl) (cl)->finalize((cl))
#define NSSD_CLIENT_CONNECT(cl) (cl)->connect((cl))
#define NSSD_CLIENT_DISCONNECT(cl) (cl)->disconnect((cl))
#define NSSD_CLIENT_WRITE(cl, p) (cl)->write((cl), (p))
#define NSSD_CLIENT_READ(cl, p) (cl)->read((cl), (p))

#endif
