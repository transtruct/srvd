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
    srvd_protocol_packet_field_finalize(i);
    srvd_protocol_packet_field_free(i);
  }

  packet->field_count = 0;
  SRVD_THREAD_MUTEX_FINALIZE(packet->field_lock);
  packet->field_head = packet->field_tail = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_add(srvd_protocol_packet_t *packet,
                                              srvd_protocol_packet_field_t *field) {
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);

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
                                                 srvd_protocol_packet_field_t *field) {
  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);

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

srvd_boolean_t srvd_protocol_packet_field_get_by_offset(const srvd_protocol_packet_t *packet,
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

srvd_boolean_t srvd_protocol_packet_field_get_by_type(const srvd_protocol_packet_t *packet,
                                                      srvd_protocol_type_t type,
                                                      srvd_protocol_packet_field_t **field) {
  srvd_protocol_packet_field_t *i;

  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(*field == NULL);

  SRVD_PROTOCOL_PACKET_FIELD_ITERATE(packet, i) {
    if(i->type == type) {
      *field = i;
      return SRVD_TRUE;
    }
  }

  /* Nothing with that type in this packet; stop. */
  return SRVD_FALSE;
}

srvd_boolean_t srvd_protocol_packet_field_get_or_add(srvd_protocol_packet_t *packet,
                                                     srvd_protocol_type_t type,
                                                     srvd_protocol_packet_field_t **field) {
  srvd_boolean_t status = SRVD_FALSE;

  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(*field == NULL);

  /* Some thread might call this function and then call it again in another thread, resulting
   * in an unfortunate error. Lock for good measure. */
  SRVD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(!srvd_protocol_packet_field_get_by_type(packet, type, field)) {
    *field = srvd_protocol_packet_field_allocate();
    if(*field == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_add: Unable to allocate memory for "
                     "field");
      goto _srvd_protocol_packet_field_get_or_add_error;
    }

    if(!srvd_protocol_packet_field_initialize(*field, type)) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_add: Unable to initialize field");
      srvd_protocol_packet_field_free(*field);
      *field = NULL;
      goto _srvd_protocol_packet_field_get_or_add_error;
    }

    if(!srvd_protocol_packet_field_add(packet, *field)) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_add: Unable to add field to packet");
      srvd_protocol_packet_field_finalize(*field);
      srvd_protocol_packet_field_free(*field);
      *field = NULL;
      goto _srvd_protocol_packet_field_get_or_add_error;
    }
  }

  status = SRVD_TRUE;

 _srvd_protocol_packet_field_get_or_add_error:

  SRVD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return status;
}

srvd_boolean_t srvd_protocol_packet_field_get_or_inject(srvd_protocol_packet_t *packet,
                                                        srvd_protocol_type_t type,
                                                        srvd_protocol_packet_field_t **field) {
  srvd_boolean_t status = SRVD_FALSE;

  SRVD_RETURN_FALSE_UNLESS(packet);
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(*field == NULL);

  /* Some thread might call this function and then call it again in another thread, resulting
   * in an unfortunate error. Lock for good measure. */
  SRVD_THREAD_MUTEX_LOCK(packet->field_lock);
  if(!srvd_protocol_packet_field_get_by_type(packet, type, field)) {
    *field = srvd_protocol_packet_field_allocate();
    if(*field == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_inject: Unable to allocate memory for "
                     "field");
      goto _srvd_protocol_packet_field_get_or_inject_error;
    }

    if(!srvd_protocol_packet_field_initialize(*field, type)) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_inject: Unable to initialize field");
      srvd_protocol_packet_field_free(*field);
      *field = NULL;
      goto _srvd_protocol_packet_field_get_or_inject_error;
    }

    if(!srvd_protocol_packet_field_inject(packet, *field)) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_get_or_inject: Unable to add field to packet");
      srvd_protocol_packet_field_finalize(*field);
      srvd_protocol_packet_field_free(*field);
      *field = NULL;
      goto _srvd_protocol_packet_field_get_or_inject_error;
    }
  }

  status = SRVD_TRUE;

 _srvd_protocol_packet_field_get_or_inject_error:

  SRVD_THREAD_MUTEX_UNLOCK(packet->field_lock);

  return status;
}

srvd_boolean_t srvd_protocol_packet_field_append(srvd_protocol_packet_t *packet,
                                                 srvd_protocol_type_t type, uint16_t size,
                                                 const void *data) {
  srvd_protocol_packet_field_t *field = NULL;

  SRVD_RETURN_FALSE_UNLESS(packet);

  if(srvd_protocol_packet_field_get_by_type(packet, type, &field)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_append: Field with type %d already exists", type);
    return SRVD_FALSE;
  }

  field = srvd_protocol_packet_field_allocate();
  if(field == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_append: Unable to allocate memory for field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_initialize(field, type)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_append: Unable to initialize field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_entry_add(field, size, data)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_append: Unable to add entry to field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_add(packet, field)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_append: Unable to add field to packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_insert(srvd_protocol_packet_t *packet,
                                                 srvd_protocol_type_t type, uint16_t size,
                                                 const void *data) {
  srvd_protocol_packet_field_t *field = NULL;

  SRVD_RETURN_FALSE_UNLESS(packet);

  if(srvd_protocol_packet_field_get_by_type(packet, type, &field)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_insert: Field with type %d already exists", type);
    return SRVD_FALSE;
  }

  field = srvd_protocol_packet_field_allocate();
  if(field == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_insert: Unable to allocate memory for field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_initialize(field, type)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_insert: Unable to initialize field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_entry_add(field, size, data)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_insert: Unable to add entry to field");
    return SRVD_FALSE;
  }

  if(!srvd_protocol_packet_field_inject(packet, field)) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_insert: Unable to add field to packet");
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_protocol_packet_field_t *srvd_protocol_packet_field_allocate(void) {
  srvd_protocol_packet_field_t *field = malloc(sizeof(srvd_protocol_packet_field_t));
  SRVD_RETURN_NULL_UNLESS(field);

  return field;
}

void srvd_protocol_packet_field_free(srvd_protocol_packet_field_t *field) {
  SRVD_RETURN_UNLESS(field);

  free(field);
}

srvd_boolean_t srvd_protocol_packet_field_initialize(srvd_protocol_packet_field_t *field,
                                                     srvd_protocol_type_t type) {
  SRVD_RETURN_FALSE_UNLESS(field);

  field->entry_count = 0;
  SRVD_THREAD_MUTEX_INITIALIZE(field->entry_lock);
  field->entry_head = field->entry_tail = NULL;
  field->type = type;
  field->next = field->previous = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_finalize(srvd_protocol_packet_field_t *field) {
  SRVD_RETURN_FALSE_UNLESS(field);

  srvd_protocol_packet_field_entry_t *i, *ni;
  for(i = field->entry_head, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    if(i->data)
      free(i->data);
    free(i);
  }

  field->entry_count = 0;
  SRVD_THREAD_MUTEX_FINALIZE(field->entry_lock);
  field->type = SRVD_PROTOCOL_NONE;
  field->entry_head = field->entry_tail = NULL;
  field->next = field->previous = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_entry_add(srvd_protocol_packet_field_t *field,
                                                    uint16_t size, const void *data) {
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_IF(size == 0 && data != NULL);
  SRVD_RETURN_FALSE_IF(size != 0 && data == NULL);

  srvd_protocol_packet_field_entry_t *entry = malloc(sizeof(srvd_protocol_packet_field_entry_t));
  if(entry == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_entry_add: Unable to allocate memory for entry");
    return SRVD_FALSE;
  }

  entry->next = entry->previous = NULL;
  entry->size = size;
  if(size > 0) {
    entry->data = malloc(size);
    if(entry->data == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_entry_add: Unable to allocate memory for "
                     "data");
      return SRVD_FALSE;
    }
    memcpy(entry->data, data, size);
  }
  else
    entry->data = NULL;

  SRVD_THREAD_MUTEX_LOCK(field->entry_lock);
  if(field->entry_head == NULL)
    field->entry_head = entry;

  entry->previous = field->entry_tail;
  if(entry->previous)
    entry->previous->next = entry;

  field->entry_tail = entry;

  field->entry_count++;
  SRVD_THREAD_MUTEX_UNLOCK(field->entry_lock);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_entry_inject(srvd_protocol_packet_field_t *field,
                                                       uint16_t size, const void *data) {
  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_IF(size == 0 && data != NULL);
  SRVD_RETURN_FALSE_IF(size != 0 && data == NULL);

  srvd_protocol_packet_field_entry_t *entry = malloc(sizeof(srvd_protocol_packet_field_entry_t));
  if(entry == NULL) {
    SRVD_LOG_ERROR("srvd_protocol_packet_field_entry_inject: Unable to allocate memory for entry");
    return SRVD_FALSE;
  }

  entry->next = entry->previous = NULL;
  entry->size = size;
  if(size > 0) {
    entry->data = malloc(size);
    if(entry->data == NULL) {
      SRVD_LOG_ERROR("srvd_protocol_packet_field_entry_inject: Unable to allocate memory for "
                     "data");
      return SRVD_FALSE;
    }
    memcpy(entry->data, data, size);
  }
  else
    entry->data = NULL;

  SRVD_THREAD_MUTEX_LOCK(field->entry_lock);
  if(field->entry_tail == NULL)
    field->entry_tail = entry;

  entry->next = field->entry_head;
  if(entry->next)
    entry->next->previous = entry;

  field->entry_head = entry;

  field->entry_count++;
  SRVD_THREAD_MUTEX_UNLOCK(field->entry_lock);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_protocol_packet_field_entry_get(const srvd_protocol_packet_field_t *field,
                                                    uint16_t offset,
                                                    srvd_protocol_packet_field_entry_t **entry) {
  srvd_protocol_packet_field_entry_t *i;
  uint16_t c;

  SRVD_RETURN_FALSE_UNLESS(field);
  SRVD_RETURN_FALSE_UNLESS(entry);
  SRVD_RETURN_FALSE_UNLESS(*entry == NULL);

  SRVD_RETURN_FALSE_UNLESS(offset < field->entry_count);

  SRVD_PROTOCOL_PACKET_FIELD_ENTRY_ITERATE_COUNT(field, i, c) {
    if(c == offset) {
      *entry = i;
      return SRVD_TRUE;
    }
  }

  /* We should never get here, really. */
  return SRVD_FALSE;
}
