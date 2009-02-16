/* packet.h: Data packet representation.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_PACKET_H
#define _NSSD_PROTOCOL_PACKET_H

#include <nssd/nssd.h>
#include <nssd/thread.h>
#include <nssd/protocol.h>

#include <arpa/inet.h>

typedef struct nssd_protocol_packet nssd_protocol_packet_t;
typedef struct nssd_protocol_packet_field nssd_protocol_packet_field_t;

struct nssd_protocol_packet {
  uint16_t field_count;
  NSSD_THREAD_MUTEX_DECLARE_UNINITIALIZED(field_lock);
  nssd_protocol_packet_field_t *field_head, *field_tail;
};

struct nssd_protocol_packet_field {
  nssd_protocol_type_t type;
  uint16_t size;
  void *data;
  nssd_protocol_packet_field_t *next, *previous;
};

#define NSSD_PROTOCOL_PACKET_FIELD_ITERATE(packet, iterator)    \
  for((iterator) = (packet)->field_head;                        \
      (iterator) != NULL;                                       \
      (iterator) = (iterator)->next)

#define NSSD_PROTOCOL_PACKET_FIELD_ITERATE_COUNT(packet, iterator, counter) \
  for((iterator) = (packet)->field_head, (counter) = 0;                 \
      (iterator) != NULL;                                               \
      (iterator) = (iterator)->next, (counter)++)

nssd_protocol_packet_t *nssd_protocol_packet_allocate(void);
void nssd_protocol_packet_free(nssd_protocol_packet_t *);
nssd_boolean_t nssd_protocol_packet_initialize(nssd_protocol_packet_t *);
nssd_boolean_t nssd_protocol_packet_finalize(nssd_protocol_packet_t *);

nssd_boolean_t nssd_protocol_packet_field_add(nssd_protocol_packet_t *, nssd_protocol_type_t,
                                              uint16_t, const void *);
nssd_boolean_t nssd_protocol_packet_field_inject(nssd_protocol_packet_t *, nssd_protocol_type_t,
                                                 uint16_t, const void *);
nssd_boolean_t nssd_protocol_packet_field_get(const nssd_protocol_packet_t *,
                                              uint16_t, nssd_protocol_packet_field_t **);

static inline
nssd_boolean_t nssd_protocol_packet_field_get_first(const nssd_protocol_packet_t *packet,
                                                    nssd_protocol_packet_field_t **field) {
  NSSD_RETURN_FALSE_UNLESS(packet);
  NSSD_RETURN_FALSE_UNLESS(field);
  NSSD_RETURN_FALSE_UNLESS(*field == NULL);

  /* There is no API for removing a field from a packet, so we can safely
   * assume that if there is a field in the head position, then it's not going
   * anywhere (unless someone decides to destroy the packet in a different
   * thread, which would be stupid). */
  *field = packet->field_head != NULL ? packet->field_head : NULL;

  return *field ? NSSD_TRUE : NSSD_FALSE;
}

static inline
size_t nssd_protocol_packet_field_size(const nssd_protocol_packet_t *packet) {
  nssd_protocol_packet_field_t *i;
  size_t size = 0;

  NSSD_RETURN_VALUE_UNLESS(packet, 0);

  NSSD_PROTOCOL_PACKET_FIELD_ITERATE(packet, i)
    size += i->size;

  return size;
}

/* Value adders (automatically preserve byte order). */
static inline
nssd_boolean_t nssd_protocol_packet_field_add_uint32(nssd_protocol_packet_t *packet,
                                                     nssd_protocol_type_t type, uint32_t data) {
  uint32_t v = htonl(data);
  return nssd_protocol_packet_field_add(packet, type, sizeof(uint32_t), &v);
}

static inline
nssd_boolean_t nssd_protocol_packet_field_add_uint16(nssd_protocol_packet_t *packet,
                                                     nssd_protocol_type_t type, uint16_t data) {
  uint16_t v = htons(data);
  return nssd_protocol_packet_field_add(packet, type, sizeof(uint16_t), &v);
}

static inline
nssd_boolean_t nssd_protocol_packet_field_add_uint8(nssd_protocol_packet_t *packet,
                                                    nssd_protocol_type_t type, uint8_t data) {
  return nssd_protocol_packet_field_add(packet, type, sizeof(uint8_t), &data);
}

static inline
nssd_boolean_t nssd_protocol_packet_field_inject_uint32(nssd_protocol_packet_t *packet,
                                                        nssd_protocol_type_t type, uint32_t data) {
  uint32_t v = htonl(data);
  return nssd_protocol_packet_field_inject(packet, type, sizeof(uint32_t), &v);
}

static inline
nssd_boolean_t nssd_protocol_packet_field_inject_uint16(nssd_protocol_packet_t *packet,
                                                        nssd_protocol_type_t type, uint16_t data) {
  uint16_t v = htons(data);
  return nssd_protocol_packet_field_inject(packet, type, sizeof(uint16_t), &v);
}

static inline
nssd_boolean_t nssd_protocol_packet_field_inject_uint8(nssd_protocol_packet_t *packet,
                                                       nssd_protocol_type_t type, uint8_t data) {
  return nssd_protocol_packet_field_inject(packet, type, sizeof(uint8_t), &data);
}

/* Value getters (automatically preserve byte order). */
static inline
nssd_boolean_t nssd_protocol_packet_field_get_uint32(nssd_protocol_packet_field_t *field,
                                                     uint32_t *data) {
  NSSD_RETURN_FALSE_UNLESS(field);
  NSSD_RETURN_FALSE_UNLESS(field->size == sizeof(uint32_t));
  NSSD_RETURN_FALSE_UNLESS(data);

  *data = ntohl(*(uint32_t *)(field->data));

  return NSSD_TRUE;
}

static inline
nssd_boolean_t nssd_protocol_packet_field_get_uint16(nssd_protocol_packet_field_t *field,
                                                     uint16_t *data) {
  NSSD_RETURN_FALSE_UNLESS(field);
  NSSD_RETURN_FALSE_UNLESS(field->size == sizeof(uint16_t));
  NSSD_RETURN_FALSE_UNLESS(data);

  *data = ntohs(*(uint16_t *)(field->data));

  return NSSD_TRUE;
}

static inline
nssd_boolean_t nssd_protocol_packet_field_get_uint8(nssd_protocol_packet_field_t *field,
                                                    uint8_t *data) {
  NSSD_RETURN_FALSE_UNLESS(field);
  NSSD_RETURN_FALSE_UNLESS(field->size == sizeof(uint8_t));
  NSSD_RETURN_FALSE_UNLESS(data);

  *data = *(uint8_t *)(field->data);

  return NSSD_TRUE;
}

#endif
