/* packet.c: Data packet representation.
 *
 * This file is part of the Pyn library.
 * Copyright (c) 2008 Transtruct. All rights reserved.
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

nssd_protocol_packet_serial_t *nssd_protocol_packet_serial_allocate(void) {
  nssd_protocol_packet_serial_t *serial = malloc(sizeof(nssd_protocol_packet_serial_t));
  assert(serial);

  return serial;
}

void nssd_protocol_packet_serial_initialize(nssd_protocol_packet_serial_t *serial) {
  assert(serial);

  serial->length = 0;
  serial->data = NULL;
}

void nssd_protocol_packet_serial_serialize(nssd_protocol_packet_serial_t *serial, const nssd_protocol_packet_t *packet) {
  assert(serial);
  assert(packet);

  size_t size;
  int i;

  /* How big do we need the packet to be? */
  size = NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE * packet->field_count;
  for(i = 0; i < packet->field_count; i++) {
    size += packet->fields[i].length;
  }

  /* Okay, now allocate it. */
  serial->length = size + NSSD_PROTOCOL_PACKET_HEADER_SIZE;
  serial->data = malloc(serial->length);

  /* And copy the data into it. */
  *(uint16_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_VERSION) =
    htons((uint16_t)NSSD_PROTOCOL_PACKET_VERSION);
  *(uint8_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_COUNT) =
    (uint8_t)packet->field_count;
  *(uint32_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_SIZE) =
    htonl((uint32_t)size);

  {
    char *p;
    for(i = 0, p = serial->data + NSSD_PROTOCOL_PACKET_HEADER_SIZE;
        i < packet->field_count;
        i++, p += packet->fields[i].length + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE) {
      *(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_TYPE) =
        htons((uint16_t)packet->fields[i].type);
      *(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_SIZE) =
        htons((uint16_t)packet->fields[i].length);
      /* The real data! */
      memcpy(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE, packet->fields[i].data, packet->fields[i].length);
    }
  }
}

void nssd_protocol_packet_serial_unserialize(nssd_protocol_packet_serial_t *serial, nssd_protocol_packet_t *packet) {
  assert(serial);
  assert(serial->data);
  assert(packet);

  uint32_t size;
  uint16_t version, fields;

  assert(serial->length >= NSSD_PROTOCOL_PACKET_HEADER_SIZE); // invalid packet received, too small

  version = ntohs(*(uint16_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_VERSION));
  assert(version == (uint16_t)NSSD_PROTOCOL_PACKET_VERSION); // packet version mismatch

  fields = *(uint8_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_COUNT);
  size = ntohl(*(uint32_t *)(serial->data + NSSD_PROTOCOL_PACKET_HEADER_OFFSET_SIZE));
  assert(size + NSSD_PROTOCOL_PACKET_HEADER_SIZE == serial->length); // invalid packet received, size mismatch
  assert(size > fields * NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE); // invalid packet, size is less than required size of field headers

  nssd_protocol_packet_fields_initialize(packet, fields);

  {
    int i;
    char *p;
    for(i = 0, p = serial->data + NSSD_PROTOCOL_PACKET_HEADER_SIZE;
        i < packet->field_count;
        p += NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE + packet->fields[i].length) {
      packet->fields[i].type = ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_TYPE));
      packet->fields[i].length = ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_SIZE));
      assert(p - serial->data + packet->fields[i].length < serial->length); // invalid packet, field length goes out of bounds

      nssd_protocol_packet_field_initialize(packet, i);
      memcpy(packet->fields[i].data, (void *)p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE, packet->fields[i].length);
    }
  }
}

void nssd_protocol_packet_serial_finalize(nssd_protocol_packet_serial_t *serial) {
  assert(serial);

  if(serial->data)
    free(serial->data);
  serial->length = 0;
}

void nssd_protocol_packet_serial_free(nssd_protocol_packet_serial_t *serial) {
  assert(serial);

  free(serial);
}
