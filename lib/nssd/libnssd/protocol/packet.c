/* packet.c: Data packet representation.
 *
 * This file is part of the nssd NSS backend.
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
  NSSD_RETURN_NULL_UNLESS(packet);

  return packet;
}

void nssd_protocol_packet_free(nssd_protocol_packet_t *packet) {
  NSSD_RETURN_UNLESS(packet);

  free(packet);
}

nssd_boolean_t nssd_protocol_packet_initialize(nssd_protocol_packet_t *packet) {
  NSSD_RETURN_FALSE_UNLESS(packet);

  packet->field_count = 0;
  NSSD_THREAD_MUTEX_INITIALIZE(packet->field_lock);
  packet->field_head = packet->field_tail = NULL;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_packet_finalize(nssd_protocol_packet_t *packet) {
  nssd_boolean_t status = NSSD_TRUE;

  NSSD_RETURN_FALSE_UNLESS(packet);

  nssd_protocol_packet_field_t *i, *ni;
  for(i = packet->field_head, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    if(i->data)
      free(i->data);
    free(i);
  }

  packet->field_count = 0;
  NSSD_THREAD_MUTEX_FINALIZE(packet->field_lock);
  packet->field_head = packet->field_tail = NULL;

  return status;
}

nssd_boolean_t nssd_protocol_packet_field_add(nssd_protocol_packet_t *packet,
                                              nssd_protocol_type_t type,
                                              uint16_t size, const void *data) {
  NSSD_RETURN_FALSE_UNLESS(packet);
  NSSD_RETURN_FALSE_IF(size == 0 && data != NULL);
  NSSD_RETURN_FALSE_IF(size != 0 && data == NULL);

  nssd_protocol_packet_field_t *field = malloc(sizeof(nssd_protocol_packet_field_t));
  if(field == NULL) {
    NSSD_LOG_ERROR("nssd_protocol_packet_field_add: Unable to allocate memory for field");
    return NSSD_FALSE;
  }

  field->next = field->previous = NULL;
  field->type = type;
  field->size = size;
  if(size > 0) {
    field->data = malloc(size);
    if(field->data == NULL) {
      NSSD_LOG_ERROR("nssd_protocol_packet_field_add: Unable to allocate memory for data");
      return NSSD_FALSE;
    }
    memcpy(field->data, data, size);
  }
  else
    field->data = NULL;

  NSSD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(packet->field_head == NULL)
    packet->field_head = field;

  field->previous = packet->field_tail;
  if(field->previous)
    field->previous->next = field;

  packet->field_tail = field;

  packet->field_count++;
  NSSD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_packet_field_inject(nssd_protocol_packet_t *packet,
                                                 nssd_protocol_type_t type,
                                                 uint16_t size, const void *data) {
  NSSD_RETURN_FALSE_UNLESS(packet);
  NSSD_RETURN_FALSE_IF(size == 0 && data != NULL);
  NSSD_RETURN_FALSE_IF(size != 0 && data == NULL);

  nssd_protocol_packet_field_t *field = malloc(sizeof(nssd_protocol_packet_field_t));
  if(field == NULL) {
    NSSD_LOG_ERROR("nssd_protocol_packet_field_inject: Unable to allocate memory for field");
    return NSSD_FALSE;
  }

  field->next = field->previous = NULL;
  field->type = type;
  field->size = size;
  if(size > 0) {
    field->data = malloc(size);
    if(field->data == NULL) {
      NSSD_LOG_ERROR("nssd_protocol_packet_field_inject: Unable to allocate memory for data");
      return NSSD_FALSE;
    }
    memcpy(field->data, data, size);
  }
  else
    field->data = NULL;

  NSSD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(packet->field_tail == NULL)
    packet->field_tail = field;

  field->next = packet->field_head;
  if(field->next)
    field->next->previous = field;

  packet->field_head = field;

  packet->field_count++;
  NSSD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return NSSD_TRUE;
}

nssd_boolean_t nssd_protocol_packet_field_get(const nssd_protocol_packet_t *packet,
                                              uint16_t offset,
                                              nssd_protocol_packet_field_t **field) {
  nssd_protocol_packet_field_t *i;
  uint16_t c;

  NSSD_RETURN_FALSE_UNLESS(packet);
  NSSD_RETURN_FALSE_UNLESS(field);
  NSSD_RETURN_FALSE_UNLESS(*field == NULL);

  NSSD_RETURN_FALSE_UNLESS(offset < packet->field_count);

  NSSD_PROTOCOL_PACKET_FIELD_ITERATE_COUNT(packet, i, c) {
    if(c == offset) {
      *field = i;
      return NSSD_TRUE;
    }
  }

  /* We should never get here, really. */
  return NSSD_FALSE;
}
