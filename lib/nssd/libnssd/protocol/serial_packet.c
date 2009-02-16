/* serial_packet.c: Serialization routines for packets.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/serial_packet.h>

nssd_protocol_serial_packet_t *nssd_protocol_serial_packet_allocate(void) {
  nssd_protocol_serial_packet_t *serial = malloc(sizeof(nssd_protocol_serial_packet_t));
  NSSD_RETURN_NULL_UNLESS(serial);

  return serial;
}

void nssd_protocol_serial_packet_free(nssd_protocol_serial_packet_t *serial) {
  NSSD_RETURN_UNLESS(serial);

  free(serial);
}

nssd_boolean_t nssd_protocol_serial_packet_finalize(nssd_protocol_serial_packet_t *serial) {
  NSSD_RETURN_FALSE_UNLESS(serial);

  if(serial->data)
    free(serial->data);
  serial->size = 0;
  serial->body_size = 0;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_serial_packet_initialize(nssd_protocol_serial_packet_t *serial) {
  NSSD_RETURN_FALSE_UNLESS(serial);

  serial->size = 0;
  serial->body_size = 0;
  serial->data = NULL;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_serial_packet_serialize(nssd_protocol_serial_packet_t *serial,
                                                     const nssd_protocol_packet_t *packet) {
  nssd_protocol_packet_field_t *i;
  char *p;
  size_t size;

  NSSD_RETURN_FALSE_UNLESS(serial);
  NSSD_RETURN_FALSE_UNLESS(packet);

  /* How big do we need the packet to be? */
  size = (NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE * packet->field_count) +
    nssd_protocol_packet_field_size(packet);

  /* Just for reference... */
  serial->field_count = packet->field_count;

  /* Okay, now allocate it. */
  serial->size = size + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE;
  serial->body_size = size;
  serial->data = malloc(serial->size);
  if(serial->data == NULL) {
    NSSD_LOG_ERROR("nssd_protocol_serial_packet_serialize: Unable to allocate memory for "
                   "packet buffer");
    return NSSD_FALSE;
  }

  memset(serial->data, 0, serial->size);

  /* And copy the data into it. */
  *(uint16_t *)(serial->data + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION) =
    htons((uint16_t)NSSD_PROTOCOL_SERIAL_PACKET_VERSION);
  *(uint16_t *)(serial->data + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT) =
    htons((uint16_t)packet->field_count);
  *(uint32_t *)(serial->data + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE) =
    htonl((uint32_t)size);

  for(i = packet->field_head, p = serial->data + NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE;
      i != NULL;
      p += i->size + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE, i = i->next) {
    *(uint16_t *)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE) =
      htons((uint16_t)i->type);
    *(uint16_t *)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_SIZE) =
      htons((uint16_t)i->size);

    /* The real data! */
    memcpy(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE, i->data, i->size);
  }

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_serial_packet_unserialize_header(nssd_protocol_serial_packet_t *serial,
                                                              nssd_protocol_packet_t *packet,
                                                              char *header) {
  NSSD_RETURN_FALSE_UNLESS(serial);
  NSSD_RETURN_FALSE_UNLESS(packet);
  NSSD_RETURN_FALSE_UNLESS(header);

  /* Make sure the packet version matches. */
  if(NSSD_PROTOCOL_SERIAL_PACKET_VERSION_GET(header) != (uint16_t)NSSD_PROTOCOL_SERIAL_PACKET_VERSION) {
    NSSD_LOG_ERROR("nssd_protocol_serial_packet_unserialize_header: Packet header version "
                   "mismatch (is the data source correct?)");
    return NSSD_FALSE;
  }

  serial->field_count = NSSD_PROTOCOL_SERIAL_PACKET_COUNT_GET(header);

  serial->body_size = NSSD_PROTOCOL_SERIAL_PACKET_SIZE_GET(header);
  serial->size = NSSD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE + serial->body_size;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_serial_packet_unserialize_body(nssd_protocol_serial_packet_t *serial,
                                                            nssd_protocol_packet_t *packet,
                                                            char *body) {
  char *p;

  /* Make sure we can legitimately read through this. */
  if(serial->body_size < (size_t)(serial->field_count * NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE)) {
    NSSD_LOG_ERROR("nssd_protocol_serial_packet_unserialize_body: Error: Field header size is "
                   "larger than total body size");
    return NSSD_FALSE;
  }

  p = body;
  while((size_t)(p - body) < serial->body_size) {
    nssd_protocol_type_t type = (nssd_protocol_type_t)ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE));
    uint16_t size = ntohs(*(uint16_t *)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_SIZE));

    /* Make sure this field header won't make us read out of bounds. */
    if((size_t)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE - body) + size > serial->body_size) {
      NSSD_LOG_ERROR("nssd_protocol_serial_packet_unserialize_body: Error: Buffer overrun while "
                     "reading packet field");
      return NSSD_FALSE;
    }

    void *data = (void *)(p + NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE);

    if(!nssd_protocol_packet_field_add(packet, type, size, data)) {
      NSSD_LOG_ERROR("nssd_protocol_serial_packet_unserialize_body: Error: Could not initialize "
                     "packet field");
      return NSSD_FALSE;
    }

    p += NSSD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE + size;
  }

  return NSSD_TRUE;
}
