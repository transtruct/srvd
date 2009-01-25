/* packet.c: Data packet representation.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/packet.h>

#include <arpa/inet.h>
#include <stdlib.h>

nssd_protocol_packet_t *nssd_protocol_packet_allocate(void) {
  nssd_protocol_packet_t *packet = malloc(sizeof(nssd_protocol_packet_t));
  assert(packet->fields);

  return packet;
}

void nssd_protocol_packet_initialize(nssd_protocol_packet_t *packet) {
  assert(packet);

  packet->field_count = 0;
  packet->fields = NULL;
}

void nssd_protocol_packet_fields_initialize(nssd_protocol_packet_t *packet, uint8_t field_count) {
  assert(packet);

  packet->field_count = field_count;
  packet->fields = calloc(field_count, sizeof(nssd_protocol_packet_field_t));
  assert(packet->fields);

  {
    int i;
    for(i = 0; i < packet->field_count; i++) {
      packet->fields[i].type = NSSD_PROTOCOL_NONE;
      packet->fields[i].length = 0;
      packet->fields[i].data = NULL;
    }
  }
}

void nssd_protocol_packet_field_initialize(nssd_protocol_packet_t *packet, int field) {
  assert(packet);
  assert(packet->field_count > field);

  packet->fields[field].data = malloc(packet->fields[field].length);
  assert(packet->fields[field].data);
}

void nssd_protocol_packet_field_finalize(nssd_protocol_packet_t *packet, int field) {
  assert(packet);
  assert(packet->field_count > field);

  assert(packet->fields[field].data);
  free(packet->fields[field].data);
}

void nssd_protocol_packet_finalize(nssd_protocol_packet_t *packet) {
  assert(packet);

  if(packet->fields) {
    int i;
    for(i = 0; i < packet->field_count; i ++) {
      if(packet->fields[i].data)
        nssd_protocol_packet_field_finalize(packet, i);
    }
    free(packet->fields);
  }
  packet->field_count = 0;
}

void nssd_protocol_packet_free(nssd_protocol_packet_t *packet) {
  assert(packet);

  free(packet);
}
