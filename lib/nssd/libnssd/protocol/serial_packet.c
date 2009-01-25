/* serial_packet.h: Serialization routines for packets.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/serial_packet.h>

nssd_protocol_serial_packet_t *nssd_protocol_serial_packet_allocate(void) {
  nssd_protocol_serial_packet_t *serial = malloc(sizeof(nssd_protocol_serial_packet_t));
  assert(serial);

  return serial;
}

void nssd_protocol_serial_packet_initialize(nssd_protocol_serial_packet_t *serial) {
  assert(serial);

  serial->length = -1;
  serial->body_length = -1;
  serial->data = NULL;
}

void nssd_protocol_serial_packet_serialize(nssd_protocol_serial_packet_t *serial, const nssd_protocol_packet_t *packet) {
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
  serial->body_length = size;
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
        p += packet->fields[i].length + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE, i++) {
      *(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_TYPE) =
        htons((uint16_t)packet->fields[i].type);
      *(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_SIZE) =
        htons((uint16_t)packet->fields[i].length);
      /* The real data! */
      memcpy(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE, packet->fields[i].data, packet->fields[i].length);
    }
  }
}

void nssd_protocol_serial_packet_unserialize_header(nssd_protocol_serial_packet_t *serial, nssd_protocol_packet_t *packet, const char *header) {
  assert(serial);
  assert(packet);
  assert(header);

  /* Make sure the packet version matches. */
  assert(NSSD_PROTOCOL_PACKET_VERSION_GET(header) == (uint16_t)NSSD_PROTOCOL_PACKET_VERSION);

  nssd_protocol_packet_fields_initialize(packet, NSSD_PROTOCOL_PACKET_COUNT_GET(header));

  serial->body_length = NSSD_PROTOCOL_PACKET_SIZE_GET(header);
  serial->length = NSSD_PROTOCOL_PACKET_HEADER_SIZE + serial->body_length;
}

void nssd_protocol_serial_packet_unserialize_body(nssd_protocol_serial_packet_t *serial, nssd_protocol_packet_t *packet, const char *body) {
  /* Make sure we can legitimately read through this. */
  assert(serial->body_length >= packet->field_count * NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE);

  int i;
  char *p;
  for(i = 0, p = (char *)body;
      i < packet->field_count;
      p += NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE + packet->fields[i].length, i++) {
    packet->fields[i].type = ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_TYPE));
    packet->fields[i].length = ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_OFFSET_SIZE));
    /* Make sure this field header won't make us read out of bounds. */
    assert((p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE - body) + packet->fields[i].length <= serial->body_length);
    
    nssd_protocol_packet_field_initialize(packet, i);
    memcpy(packet->fields[i].data, (void *)p + NSSD_PROTOCOL_PACKET_FIELD_HEADER_SIZE, packet->fields[i].length);
  }
}

void nssd_protocol_serial_packet_finalize(nssd_protocol_serial_packet_t *serial) {
  assert(serial);

  if(serial->data)
    free(serial->data);
  serial->length = -1;
  serial->body_length = -1;
}

void nssd_protocol_serial_packet_free(nssd_protocol_serial_packet_t *serial) {
  assert(serial);

  free(serial);
}
