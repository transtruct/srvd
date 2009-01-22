/* packet.h: Data packet representation.
 *
 * This file is part of the Pyn library.
 * Copyright (c) 2008 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_PACKET_H
#define _NSSD_PROTOCOL_PACKET_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>

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
 */

/* Protocol changes:
 * - 100: Initial version.
 */
#define NSSD_PROTOCOL_PACKET_VERSION 100

/* All packets are at least the size of the header, which is 8 bytes. */
#define NSSD_PROTOCOL_PACKET_HEADER_SIZE 8

/* Additionally, each field has an overhead of 4 bytes. */
#define NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE 4

/* Offsets for reading the structures. */
#define NSSD_PROTOCOL_PACKET_HEADER_OFFSET_VERSION 0
#define NSSD_PROTOCOL_PACKET_HEADER_OFFSET_COUNT 2
#define NSSD_PROTOCOL_PACKET_HEADER_OFFSET_SIZE 4

#define NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_TYPE 0
#define NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_SIZE 2

typedef struct nssd_protocol_packet nssd_protocol_packet_t;
typedef struct nssd_protocol_packet_field nssd_protocol_packet_field_t;
typedef struct nssd_protocol_packet_serial nssd_protocol_packet_serial_t;

struct nssd_protocol_packet {
  uint16_t field_count;
  nssd_protocol_packet_field_t *fields;
};

struct nssd_protocol_packet_field {
  nssd_protocol_type_t type;
  uint16_t length;
  nssd_protocol_packet_field_t *next;
  void *data;
};

struct nssd_protocol_packet_serial {
  size_t length;
  char *data;
};

nssd_protocol_packet_t *nssd_protocol_packet_allocate(void);
void nssd_protocol_packet_initialize(nssd_protocol_packet_t *);
void nssd_protocol_packet_fields_initialize(nssd_protocol_packet_t *, uint8_t);
void nssd_protocol_packet_field_initialize(nssd_protocol_packet_t *, int);
void nssd_protocol_packet_field_finalize(nssd_protocol_packet_t *, int);
void nssd_protocol_packet_finalize(nssd_protocol_packet_t *);
void nssd_protocol_packet_free(nssd_protocol_packet_t *);

nssd_protocol_packet_serial_t *nssd_protocol_packet_serial_allocate(void);
void nssd_protocol_packet_serial_initialize(nssd_protocol_packet_serial_t *);
void nssd_protocol_packet_serial_serialize(nssd_protocol_packet_serial_t *, const nssd_protocol_packet_t *);
void nssd_protocol_packet_serial_unserialize(nssd_protocol_packet_serial_t *, nssd_protocol_packet_t *);
void nssd_protocol_packet_serial_finalize(nssd_protocol_packet_serial_t *);
void nssd_protocol_packet_serial_free(nssd_protocol_packet_serial_t *);

static inline size_t nssd_protocol_packet_fields_length(nssd_protocol_packet_t *packet) {
  assert(packet);

  int i;
  size_t length;
  for(i = 0; i < packet->field_count; i++) {
    length += packet->fields[i].length;
  }

  return length;
}

#endif
