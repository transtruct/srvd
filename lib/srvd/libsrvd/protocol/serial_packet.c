/* serial_packet.c: Serialization routines for packets.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/protocol/serial_packet.h>

srvd_protocol_serial_packet_t *srvd_protocol_serial_packet_allocate(void) {
  srvd_protocol_serial_packet_t *serial = malloc(sizeof(srvd_protocol_serial_packet_t));
  SRVD_RETURN_NULL_UNLESS(serial);

  return serial;
}

void srvd_protocol_serial_packet_free(srvd_protocol_serial_packet_t *serial) {
  SRVD_RETURN_UNLESS(serial);

  free(serial);
}

srvd_boolean_t srvd_protocol_serial_packet_finalize(srvd_protocol_serial_packet_t *serial) {
  SRVD_RETURN_FALSE_UNLESS(serial);

  if(serial->data)
    free(serial->data);
  serial->size = 0;
  serial->body_size = 0;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_serial_packet_initialize(srvd_protocol_serial_packet_t *serial) {
  SRVD_RETURN_FALSE_UNLESS(serial);

  serial->size = 0;
  serial->body_size = 0;
  serial->data = NULL;

  return SRVD_TRUE;
}

static inline size_t _srvd_protocol_serial_packet_body_size(const srvd_protocol_packet_t *packet) {
  size_t size = SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE * packet->field_count;
  srvd_protocol_packet_field_t *field;

  SRVD_PROTOCOL_PACKET_FIELD_ITERATE(packet, field) {
    srvd_protocol_packet_field_entry_t *entry;

    size += SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE * field->entry_count;
    SRVD_PROTOCOL_PACKET_FIELD_ENTRY_ITERATE(field, entry)
      size += entry->size;
  }

  return size;
}

srvd_boolean_t srvd_protocol_serial_packet_serialize(srvd_protocol_serial_packet_t *serial,
                                                     const srvd_protocol_packet_t *packet) {
  srvd_protocol_packet_field_t *field;
  char *p;
  size_t body_size;

  SRVD_RETURN_FALSE_UNLESS(serial);
  SRVD_RETURN_FALSE_UNLESS(packet);

  /* How big do we need the packet to be? */
  body_size = _srvd_protocol_serial_packet_body_size(packet);

  /* Just for reference... */
  serial->field_count = packet->field_count;

  /* Okay, now allocate it. */
  serial->size = body_size + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE;
  serial->body_size = body_size;
  serial->data = malloc(serial->size);
  if(serial->data == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_serial_packet_serialize: Unable to allocate memory for "
                   "packet buffer");
    return SRVD_FALSE;
  }

  memset(serial->data, 0, serial->size);

  /* And copy the data into it. */
  *(uint16_t *)(serial->data + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_VERSION) =
    htons((uint16_t)SRVD_PROTOCOL_SERIAL_PACKET_VERSION);
  *(uint16_t *)(serial->data + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_COUNT) =
    htons((uint16_t)packet->field_count);
  *(uint32_t *)(serial->data + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_OFFSET_SIZE) =
    htonl((uint32_t)serial->body_size);

  for(field = packet->field_head, p = serial->data + SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE;
      field != NULL;
      field = field->next) {
    srvd_protocol_packet_field_entry_t *entry;

    /* Headers. */
    *(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE) =
      htons((uint16_t)field->type);
    *(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_COUNT) =
      htons((uint16_t)field->entry_count);

    p += SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE;

    for(entry = field->entry_head;
        entry != NULL;
        p += entry->size, entry = entry->next) {
      /* Headers. */
      *(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_OFFSET_SIZE) =
        htons((uint16_t)entry->size);

      p += SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE;

      /* The real data! */
      memcpy(p, entry->data, entry->size);
    }
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_serial_packet_unserialize_header(srvd_protocol_serial_packet_t *serial,
                                                              srvd_protocol_packet_t *packet,
                                                              char *header) {
  SRVD_RETURN_FALSE_UNLESS(serial);
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(header);

  /* Make sure the packet version matches. */
  if(SRVD_PROTOCOL_SERIAL_PACKET_VERSION_GET(header) != (uint16_t)SRVD_PROTOCOL_SERIAL_PACKET_VERSION) {
    SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_header: Packet header version "
                   "mismatch (is the data source correct?)");
    return SRVD_FALSE;
  }

  serial->field_count = SRVD_PROTOCOL_SERIAL_PACKET_COUNT_GET(header);

  serial->body_size = SRVD_PROTOCOL_SERIAL_PACKET_SIZE_GET(header);
  serial->size = SRVD_PROTOCOL_SERIAL_PACKET_HEADER_SIZE + serial->body_size;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_serial_packet_unserialize_body(srvd_protocol_serial_packet_t *serial,
                                                            srvd_protocol_packet_t *packet,
                                                            char *body) {
  char *p;
  uint16_t i;

  /* Make sure we can legitimately read through this. */
  if(serial->body_size < (size_t)(serial->field_count * SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE)) {
    SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Error: Field header size is "
                   "larger than total body size");
    return SRVD_FALSE;
  }

  p = body;
  for(i = 0; i < serial->field_count; i++) {
    uint16_t entry, entry_count;
    srvd_protocol_packet_field_t *field = NULL;
    srvd_protocol_type_t type;

    if((size_t)(p - body) >= serial->body_size) {
      SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Reading after end of valid data");
      return SRVD_FALSE;
    }

    type = (srvd_protocol_type_t)ntohs(*(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_TYPE));
    entry_count = ntohs(*(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_OFFSET_COUNT));

    /* We need to be able to read at least through the entry headers as well. */
    if((size_t)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE - body) +
       (entry_count * SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE) > serial->body_size) {
      SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Error: Field entry header "
                     "size is larger than total body size");
      return SRVD_FALSE;
    }

    if(!srvd_protocol_packet_field_get_or_add(packet, type, &field)) {
      SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Unable to get field instance");
      return SRVD_FALSE;
    }

    p += SRVD_PROTOCOL_SERIAL_PACKET_FIELD_HEADER_SIZE;

    for(entry = 0; entry < entry_count; entry++) {
      uint16_t size = ntohs(*(uint16_t *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_OFFSET_SIZE));

      /* Make sure this entry header won't make us read out of bounds. */
      if((size_t)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE - body) + size > serial->body_size) {
        SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Error: Buffer overrun while "
                       "reading packet field entry");
        srvd_protocol_packet_field_finalize(field);
        srvd_protocol_packet_field_free(field);
        return SRVD_FALSE;
      }

      void *data = (void *)(p + SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE);

      if(!srvd_protocol_packet_field_entry_add(field, size, data)) {
        SRVD_LOG_ERROR("srvd_protocol_serial_packet_unserialize_body: Error: Could not initialize "
                       "packet field");
        srvd_protocol_packet_field_finalize(field);
        srvd_protocol_packet_field_free(field);
        return SRVD_FALSE;
      }

      p += SRVD_PROTOCOL_SERIAL_PACKET_FIELD_ENTRY_HEADER_SIZE + size;
    }
  }

  return SRVD_TRUE;
}
