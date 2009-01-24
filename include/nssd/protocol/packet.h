/* packet.h: Data packet representation.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_PACKET_H
#define _NSSD_PROTOCOL_PACKET_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>

typedef struct nssd_protocol_packet nssd_protocol_packet_t;
typedef struct nssd_protocol_packet_field nssd_protocol_packet_field_t;

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

nssd_protocol_packet_t *nssd_protocol_packet_allocate(void);
void nssd_protocol_packet_initialize(nssd_protocol_packet_t *);
void nssd_protocol_packet_fields_initialize(nssd_protocol_packet_t *, uint8_t);
void nssd_protocol_packet_field_initialize(nssd_protocol_packet_t *, int);
void nssd_protocol_packet_field_finalize(nssd_protocol_packet_t *, int);
void nssd_protocol_packet_finalize(nssd_protocol_packet_t *);
void nssd_protocol_packet_free(nssd_protocol_packet_t *);

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
