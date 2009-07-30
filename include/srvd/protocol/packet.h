/* packet.h: Data packet representation.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_PROTOCOL_PACKET_H
#define _SRVD_PROTOCOL_PACKET_H

#include <srvd/srvd.h>
#include <srvd/thread.h>
#include <srvd/protocol.h>

#include <arpa/inet.h>

typedef struct srvd_protocol_packet srvd_protocol_packet_t;
typedef struct srvd_protocol_packet_field srvd_protocol_packet_field_t;
typedef struct srvd_protocol_packet_field_entry srvd_protocol_packet_field_entry_t;

struct srvd_protocol_packet {
  uint16_t field_count;
  SRVD_THREAD_MUTEX_DECLARE_UNINITIALIZED(field_lock);
  srvd_protocol_packet_field_t *field_head, *field_tail;
};

struct srvd_protocol_packet_field {
  uint16_t entry_count;
  SRVD_THREAD_MUTEX_DECLARE_UNINITIALIZED(entry_lock);
  srvd_protocol_packet_field_entry_t *entry_head, *entry_tail;
  srvd_protocol_type_t type;
  srvd_protocol_packet_field_t *next, *previous;
};

struct srvd_protocol_packet_field_entry {
  uint16_t size;
  void *data;
  srvd_protocol_packet_field_entry_t *next, *previous;
};

#define SRVD_PROTOCOL_PACKET_FIELD_ITERATE(packet, iterator)    \
  for((iterator) = (packet)->field_head;                        \
      (iterator) != NULL;                                       \
      (iterator) = (iterator)->next)

#define SRVD_PROTOCOL_PACKET_FIELD_ITERATE_COUNT(packet, iterator, counter) \
  for((iterator) = (packet)->field_head, (counter) = 0;                 \
      (iterator) != NULL;                                               \
      (iterator) = (iterator)->next, (counter)++)

#define SRVD_PROTOCOL_PACKET_FIELD_ENTRY_ITERATE(field, iterator) \
  for((iterator) = (field)->entry_head;                           \
      (iterator) != NULL;                                         \
      (iterator) = (iterator)->next)

#define SRVD_PROTOCOL_PACKET_FIELD_ENTRY_ITERATE_COUNT(field, iterator, counter) \
  for((iterator) = (field)->entry_head, (counter) = 0;                  \
      (iterator) != NULL;                                               \
      (iterator) = (iterator)->next, (counter)++)

srvd_protocol_packet_t *srvd_protocol_packet_allocate(void);
void srvd_protocol_packet_free(srvd_protocol_packet_t *);
srvd_boolean_t srvd_protocol_packet_initialize(srvd_protocol_packet_t *);
srvd_boolean_t srvd_protocol_packet_finalize(srvd_protocol_packet_t *);

srvd_boolean_t srvd_protocol_packet_field_add(srvd_protocol_packet_t *,
                                              srvd_protocol_packet_field_t *);
srvd_boolean_t srvd_protocol_packet_field_inject(srvd_protocol_packet_t *,
                                                 srvd_protocol_packet_field_t *);
srvd_boolean_t srvd_protocol_packet_field_get_by_offset(const srvd_protocol_packet_t *, uint16_t,
                                                        srvd_protocol_packet_field_t **);
srvd_boolean_t srvd_protocol_packet_field_get_by_type(const srvd_protocol_packet_t *,
                                                      srvd_protocol_type_t,
                                                      srvd_protocol_packet_field_t **);

srvd_boolean_t srvd_protocol_packet_field_get_or_create(srvd_protocol_packet_t *,
                                                        srvd_protocol_type_t,
                                                        srvd_protocol_packet_field_t **);

srvd_boolean_t srvd_protocol_packet_field_append(srvd_protocol_packet_t *,
                                                 srvd_protocol_type_t, uint16_t, const void *);
srvd_boolean_t srvd_protocol_packet_field_insert(srvd_protocol_packet_t *,
                                                 srvd_protocol_type_t, uint16_t, const void *);

srvd_protocol_packet_field_t *srvd_protocol_packet_field_allocate(void);
void srvd_protocol_packet_field_free(srvd_protocol_packet_field_t *);
srvd_boolean_t srvd_protocol_packet_field_initialize(srvd_protocol_packet_field_t *,
                                                     srvd_protocol_type_t);
srvd_boolean_t srvd_protocol_packet_field_finalize(srvd_protocol_packet_field_t *);

srvd_boolean_t srvd_protocol_packet_field_entry_add(srvd_protocol_packet_field_t *, uint16_t,
                                                    const void *);
srvd_boolean_t srvd_protocol_packet_field_entry_inject(srvd_protocol_packet_field_t *, uint16_t,
                                                       const void *);
srvd_boolean_t srvd_protocol_packet_field_entry_get(const srvd_protocol_packet_field_t *,
                                                    uint16_t, srvd_protocol_packet_field_entry_t **);

static inline
srvd_boolean_t srvd_protocol_packet_field_get_first(const srvd_protocol_packet_t *packet,
                                                    srvd_protocol_packet_field_t **field) {
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(*field == NULL);

  /* There is no API for removing a field from a packet, so we can safely
   * assume that if there is a field in the head position, then it's not going
   * anywhere (unless someone decides to destroy the packet in a different
   * thread, which would be stupid). */
  *field = packet->field_head != NULL ? packet->field_head : NULL;

  return *field ? SRVD_TRUE : SRVD_FALSE;
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_get_first(const srvd_protocol_packet_field_t *field,
                                                          srvd_protocol_packet_field_entry_t **entry) {
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(entry);
  SRVD_RETURN_FALSE_UNLESS(*entry == NULL);

  /* There is no API for removing a field from a packet, so we can safely
   * assume that if there is a field in the head position, then it's not going
   * anywhere (unless someone decides to destroy the packet in a different
   * thread, which would be stupid). */
  *entry = field->entry_head != NULL ? field->entry_head : NULL;

  return *entry ? SRVD_TRUE : SRVD_FALSE;
}

/* Value adders (automatically preserve byte order). */
static inline
srvd_boolean_t srvd_protocol_packet_field_entry_add_uint32(srvd_protocol_packet_field_t *field,
                                                           uint32_t data) {
  uint32_t v = htonl(data);
  return srvd_protocol_packet_field_entry_add(field, sizeof(uint32_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_add_uint16(srvd_protocol_packet_field_t *field,
                                                           uint16_t data) {
  uint16_t v = htons(data);
  return srvd_protocol_packet_field_entry_add(field, sizeof(uint16_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_add_uint8(srvd_protocol_packet_field_t *field,
                                                          uint8_t data) {
  return srvd_protocol_packet_field_entry_add(field, sizeof(uint8_t), &data);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_inject_uint32(srvd_protocol_packet_field_t *field,
                                                              uint32_t data) {
  uint32_t v = htonl(data);
  return srvd_protocol_packet_field_entry_inject(field, sizeof(uint32_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_inject_uint16(srvd_protocol_packet_field_t *field,
                                                              uint16_t data) {
  uint16_t v = htons(data);
  return srvd_protocol_packet_field_entry_inject(field, sizeof(uint16_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_inject_uint8(srvd_protocol_packet_field_t *field,
                                                             uint8_t data) {
  return srvd_protocol_packet_field_entry_inject(field, sizeof(uint8_t), &data);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_append_uint32(srvd_protocol_packet_t *packet,
                                                        srvd_protocol_type_t type, uint32_t data) {
  uint32_t v = htonl(data);
  return srvd_protocol_packet_field_append(packet, type, sizeof(uint32_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_append_uint16(srvd_protocol_packet_t *packet,
                                                        srvd_protocol_type_t type, uint16_t data) {
  uint16_t v = htons(data);
  return srvd_protocol_packet_field_append(packet, type, sizeof(uint16_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_append_uint8(srvd_protocol_packet_t *packet,
                                                       srvd_protocol_type_t type, uint8_t data) {
  return srvd_protocol_packet_field_append(packet, type, sizeof(uint8_t), &data);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_insert_uint32(srvd_protocol_packet_t *packet,
                                                        srvd_protocol_type_t type, uint32_t data) {
  uint32_t v = htonl(data);
  return srvd_protocol_packet_field_insert(packet, type, sizeof(uint32_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_insert_uint16(srvd_protocol_packet_t *packet,
                                                        srvd_protocol_type_t type, uint16_t data) {
  uint16_t v = htons(data);
  return srvd_protocol_packet_field_insert(packet, type, sizeof(uint16_t), &v);
}

static inline
srvd_boolean_t srvd_protocol_packet_field_insert_uint8(srvd_protocol_packet_t *packet,
                                                       srvd_protocol_type_t type, uint8_t data) {
  return srvd_protocol_packet_field_insert(packet, type, sizeof(uint8_t), &data);
}

/* Value getters (automatically preserve byte order). */
static inline
srvd_boolean_t srvd_protocol_packet_field_entry_get_uint32(srvd_protocol_packet_field_entry_t *entry,
                                                           uint32_t *data) {
  SRVD_RETURN_FALSE_UNLESS(entry);
  SRVD_RETURN_FALSE_UNLESS(entry->size == sizeof(uint32_t));
  SRVD_RETURN_FALSE_UNLESS(data);

  *data = ntohl(*(uint32_t *)(entry->data));

  return SRVD_TRUE;
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_get_uint16(srvd_protocol_packet_field_entry_t *entry,
                                                           uint16_t *data) {
  SRVD_RETURN_FALSE_UNLESS(entry);
  SRVD_RETURN_FALSE_UNLESS(entry->size == sizeof(uint16_t));
  SRVD_RETURN_FALSE_UNLESS(data);

  *data = ntohs(*(uint16_t *)(entry->data));

  return SRVD_TRUE;
}

static inline
srvd_boolean_t srvd_protocol_packet_field_entry_get_uint8(srvd_protocol_packet_field_entry_t *entry,
                                                          uint8_t *data) {
  SRVD_RETURN_FALSE_UNLESS(entry);
  SRVD_RETURN_FALSE_UNLESS(entry->size == sizeof(uint8_t));
  SRVD_RETURN_FALSE_UNLESS(data);

  *data = *(uint8_t *)(entry->data);

  return SRVD_TRUE;
}

#endif
