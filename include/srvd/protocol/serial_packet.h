/* serial_packet.h: Serialization routines for packets.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_PROTOCOL_SERIAL_PACKET_H
#define _SRVD_PROTOCOL_SERIAL_PACKET_H

#include <srvd/srvd.h>
#include <srvd/protocol/packet.h>

/* Packet format:
 *
 * 0       8       16      24      32
 * +---------------+---------------+
 * | version       | field count   | <-- Header
 * +---------------+---------------+
 * | size                          |
 * +-------------------------------+
 * +---------------+---------------+
 * | type          | entry count   | <-- Fields
 * +---------------+---------------+
 * +---------------+---------------+
 * | size          | data          | <-- Entry
 * +---------------+               |
 * |               .               |
 * |               .               |
 * +-------------------------------+
 * +---------------+---------------+
 * | size          | data          | <-- Entry
 * +---------------+               |
 * |               .               |
 * |               .               |
 * +-------------------------------+
 *                 .
 *                 .
 *                 .
 */

/* Protocol changes:
 * - 110: Support multiple entries per field.
 * - 100: Initial version.
 */
#define SRVD_PROTOCOL_SERIAL_PACKET_VERSION 110

/* All packets are at least the size of the header, which is 8 bytes. */
#define SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE 8

/* Additionally, each field has an overhead of 4 bytes. */
#define SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE 4

/* And each field entry has an overhead of 2 bytes. */
#define SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE 2

/* Offsets for reading the structures. */
#define SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION 0
#define SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT 2
#define SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE 4

/* Macros for getting and setting data from the structures. */
#define SRVD_PROTOCOL_SERIAL_PACKET_VERSION_GET(buffer)                 \
  (ntohs(*(uint16_t *)((buffer) + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION)))
#define SRVD_PROTOCOL_SERIAL_PACKET_COUNT_GET(buffer)                   \
  (ntohs(*(uint16_t *)((buffer) + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT)))
#define SRVD_PROTOCOL_SERIAL_PACKET_SIZE_GET(buffer)                    \
  (ntohl(*(uint32_t *)((buffer) + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE)))

#define SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE 0
#define SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_COUNT 2

#define SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_OFFSET_SIZE 0

typedef struct srvd_protocol_serial_packet srvd_protocol_serial_packet_t;

struct srvd_protocol_serial_packet {
  size_t size, body_size;
  uint16_t field_count;
  char *data;
};

srvd_protocol_serial_packet_t *srvd_protocol_serial_packet_allocate(void);
void srvd_protocol_serial_packet_free(srvd_protocol_serial_packet_t *);
srvd_boolean_t srvd_protocol_serial_packet_initialize(srvd_protocol_serial_packet_t *);
srvd_boolean_t srvd_protocol_serial_packet_finalize(srvd_protocol_serial_packet_t *);
srvd_boolean_t srvd_protocol_serial_packet_serialize(srvd_protocol_serial_packet_t *, const srvd_protocol_packet_t *);
srvd_boolean_t srvd_protocol_serial_packet_unserialize_header(srvd_protocol_serial_packet_t *, srvd_protocol_packet_t *, char *header);
srvd_boolean_t srvd_protocol_serial_packet_unserialize_body(srvd_protocol_serial_packet_t *, srvd_protocol_packet_t *, char *body);

#endif
