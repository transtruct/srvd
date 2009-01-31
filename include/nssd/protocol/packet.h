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
#include <nssd/protocol.h>

#include <arpa/inet.h>

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

/* Setters */
#define NSSD_PROTOCOL_PACKET__FIELD_SET_INITIALIZE(packet, field, field_type, value_length) \
  do { \
    assert((packet)->field_count > (field));                            \
                                                                        \
    (packet)->fields[(field)].type = (field_type);                      \
    (packet)->fields[(field)].length = (value_length);                  \
                                                                        \
    nssd_protocol_packet_field_initialize((packet), (field));           \
  } while(0)

#define NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(packet, field, field_type, value_type, value) \
  do {                                                                  \
    NSSD_PROTOCOL_PACKET__FIELD_SET_INITIALIZE(packet, field, field_type, sizeof(value_type)); \
    *(value_type *)(packet)->fields[(field)].data = (value);            \
  } while(0)

#define NSSD_PROTOCOL_PACKET_FIELD_SET_UINT32(packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(packet, field, field_type, uint32_t, htonl(value))

#define NSSD_PROTOCOL_PACKET_FIELD_SET_UINT16(packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(packet, field, field_type, uint16_t, htons(value))

#define NSSD_PROTOCOL_PACKET_FIELD_SET_UINT8(packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(packet, field, field_type, uint8_t, value)

#define NSSD_PROTOCOL_PACKET_FIELD_SET_ARRAY(packet, field, field_type, value, value_length) \
  do {                                                                  \
    NSSD_PROTOCOL_PACKET__FIELD_SET_INITIALIZE(packet, field, field_type, value_length); \
    memcpy((packet)->fields[(field)].data, (void *)(value), (packet)->fields[(field)].length); \
  } while(0)

#define NSSD_PROTOCOL_PACKET_FIELD_SET_STRING(packet, field, field_type, value, value_length) \
  do {                                                                  \
    NSSD_PROTOCOL_PACKET__FIELD_SET_INITIALIZE(packet, field, field_type, value_length); \
    strncpy((packet)->fields[(field)].data, (char *)(value), (packet)->fields[(field)].length); \
  } while(0)

/* Getters */

#define NSSD_PROTOCOL_PACKET__FIELD_GET_INITIALIZE(packet, field)       \
  assert((packet)->field_count > field)

#define NSSD_PROTOCOL_PACKET_FIELD_GET_SCALAR(packet, field, value_type) \
  (*(value_type *)(packet)->fields[(field)].data)

#define NSSD_PROTOCOL_PACKET_FIELD_GET_UINT32(packet, field)            \
  ntohl(NSSD_PROTOCOL_PACKET_FIELD_GET_SCALAR(packet, field, uint32_t))

#define NSSD_PROTOCOL_PACKET_FIELD_GET_UINT16(packet, field)            \
  ntohs(NSSD_PROTOCOL_PACKET_FIELD_GET_SCALAR(packet, field, uint16_t))

#define NSSD_PROTOCOL_PACKET_FIELD_GET_UINT8(packet, field)             \
  NSSD_PROTOCOL_PACKET_FIELD_GET_SCALAR(packet, field, uint8_t)

#define NSSD_PROTOCOL_PACKET_FIELD_GET_ARRAY(location, packet, field)   \
  do {                                                                  \
    NSSD_PROTOCOL_PACKET__FIELD_GET_INITIALIZE(packet, field);          \
    memcpy((location),                                                  \
           (packet)->fields[(field)].data, (packet)->fields[(field)].length); \
  } while(0)

#define NSSD_PROTOCOL_PACKET_FIELD_GET_STRING(location, packet, field)  \
  do {                                                                  \
    NSSD_PROTOCOL_PACKET__FIELD_GET_INITIALIZE(packet, field);          \
    strncpy((char *)(location),                                         \
            (packet)->fields[(field)].data, (packet)->fields[(field)].length); \
  } while(0)

#endif
