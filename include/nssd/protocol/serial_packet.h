/* serial_packet.h: Serialization routines for packets.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_SERIAL_PACKET_H
#define _NSSD_PROTOCOL_SERIAL_PACKET_H

#include <nssd/nssd.h>
#include <nssd/protocol/packet.h>

/* Packet format:
 *
 * 0       8       16      24      32
 * +-------------------------------+
 * | version       | field count   | <-- Header
 * +---------------+---------------+
 * | size                          |
 * +-------------------------------+
 * +-------------------------------+
 * | type          | size          | <-- Fields
 * +---------------+---------------+
 * | data                          |
 * |               .               |
 * |               .               |
 * |               .               |
 * +-------------------------------+
 * +-------------------------------+
 * | type          | size          |
 * +---------------+---------------+
 * | data                          |
 * |               .               |
 * |               .               |
 * |               .               |
 * +-------------------------------+
 *                 .
 *                 .
 *                 .
 */

/* Protocol changes:
 * - 100: Initial version.
 */
#define NSSD_PROTOCOL_SERIAL_PACKET_VERSION 100

/* All packets are at least the size of the header, which is 8 bytes. */
#define NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE 8

/* Additionally, each field has an overhead of 4 bytes. */
#define NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE 4

/* Offsets for reading the structures. */
#define NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION 0
#define NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT 2
#define NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE 4

/* Macros for getting data from the structures. */
#define NSSD_PROTOCOL_SERIAL_PACKET_VERSION_GET(buffer)                 \
  (ntohs(*(uint16_t *)((buffer) + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION)))
#define NSSD_PROTOCOL_SERIAL_PACKET_COUNT_GET(buffer)                   \
  (ntohs(*(uint16_t *)((buffer) + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT)))
#define NSSD_PROTOCOL_SERIAL_PACKET_SIZE_GET(buffer)                    \
  (ntohl(*(uint32_t *)((buffer) + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE)))

#define NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE 0
#define NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_SIZE 2

typedef struct nssd_protocol_serial_packet nssd_protocol_serial_packet_t;

struct nssd_protocol_serial_packet {
  size_t size, body_size;
  uint16_t field_count;
  char *data;
};

nssd_protocol_serial_packet_t *nssd_protocol_serial_packet_allocate(void);
void nssd_protocol_serial_packet_free(nssd_protocol_serial_packet_t *);
nssd_boolean_t nssd_protocol_serial_packet_initialize(nssd_protocol_serial_packet_t *);
nssd_boolean_t nssd_protocol_serial_packet_finalize(nssd_protocol_serial_packet_t *);
nssd_boolean_t nssd_protocol_serial_packet_serialize(nssd_protocol_serial_packet_t *, const nssd_protocol_packet_t *);
nssd_boolean_t nssd_protocol_serial_packet_unserialize_header(nssd_protocol_serial_packet_t *, nssd_protocol_packet_t *, char *header);
nssd_boolean_t nssd_protocol_serial_packet_unserialize_body(nssd_protocol_serial_packet_t *, nssd_protocol_packet_t *, char *body);

#endif
