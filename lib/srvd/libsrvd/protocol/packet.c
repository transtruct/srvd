/* packet.c: Data packet representation.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/protocol/packet.h>

#include <arpa/inet.h>
#include <stdlib.h>

srvd_protocol_packet_t *srvd_protocol_packet_allocate(void) {
  srvd_protocol_packet_t *packet = malloc(sizeof(srvd_protocol_packet_t));
  SRVD_RETURN_NULL_UNLESS(packet);

  return packet;
}

void srvd_protocol_packet_free(srvd_protocol_packet_t *packet) {
  SRVD_RETURN_UNLESS(packet);

  free(packet);
}

srvd_boolean_t srvd_protocol_packet_initialize(srvd_protocol_packet_t *packet) {
  SRVD_RETURN_FALSE_UNLESS(packet);

  packet->field_count = 0;
  SRVD_THREAD_MUTEX_INITIALIZE(packet->field_lock);
  packet->field_head = packet->field_tail = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_finalize(srvd_protocol_packet_t *packet) {
  SRVD_RETURN_FALSE_UNLESS(packet);

  srvd_protocol_packet_field_t *i, *ni;
  for(i = packet->field_head, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    if(i->data)
      free(i->data);
    free(i);
  }

  packet->field_count = 0;
  SRVD_THREAD_MUTEX_FINALIZE(packet->field_lock);
  packet->field_head = packet->field_tail = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_add(srvd_protocol_packet_t *packet,
                                              srvd_protocol_type_t type,
                                              uint16_t size, const void *data) {
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_IF(size == 0 && data != NULL);
  SRVD_RETURN_FALSE_IF(size != 0 && data == NULL);

  srvd_protocol_packet_field_t *field = malloc(sizeof(srvd_protocol_packet_field_t));
  if(field == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_add: Unable to allocate memory for field");
    return SRVD_FALSE;
  }

  field->next = field->previous = NULL;
  field->type = type;
  field->size = size;
  if(size > 0) {
    field->data = malloc(size);
    if(field->data == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_add: Unable to allocate memory for data");
      return SRVD_FALSE;
    }
    memcpy(field->data, data, size);
  }
  else
    field->data = NULL;

  SRVD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(packet->field_head == NULL)
    packet->field_head = field;

  field->previous = packet->field_tail;
  if(field->previous)
    field->previous->next = field;

  packet->field_tail = field;

  packet->field_count++;
  SRVD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_inject(srvd_protocol_packet_t *packet,
                                                 srvd_protocol_type_t type,
                                                 uint16_t size, const void *data) {
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_IF(size == 0 && data != NULL);
  SRVD_RETURN_FALSE_IF(size != 0 && data == NULL);

  srvd_protocol_packet_field_t *field = malloc(sizeof(srvd_protocol_packet_field_t));
  if(field == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_inject: Unable to allocate memory for field");
    return SRVD_FALSE;
  }

  field->next = field->previous = NULL;
  field->type = type;
  field->size = size;
  if(size > 0) {
    field->data = malloc(size);
    if(field->data == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_inject: Unable to allocate memory for data");
      return SRVD_FALSE;
    }
    memcpy(field->data, data, size);
  }
  else
    field->data = NULL;

  SRVD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(packet->field_tail == NULL)
    packet->field_tail = field;

  field->next = packet->field_head;
  if(field->next)
    field->next->previous = field;

  packet->field_head = field;

  packet->field_count++;
  SRVD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_get(const srvd_protocol_packet_t *packet,
                                              uint16_t offset,
                                              srvd_protocol_packet_field_t **field) {
  srvd_protocol_packet_field_t *i;
  uint16_t c;

  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(*field == NULL);

  SRVD_RETURN_FALSE_UNLESS(offset < packet->field_count);

  SRVD_PROTOCOL_PACKET_FIELD_ITERATE_COUNT(packet, i, c) {
    if(c == offset) {
      *field = i;
      return SRVD_TRUE;
    }
  }

  /* We should never get here, really. */
  return SRVD_FALSE;
}
