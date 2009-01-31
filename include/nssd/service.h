/* service.h: Support for services.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_SERVICE_H
#define _NSSD_SERVICE_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>
#include <nssd/protocol/packet.h>

/* Request */
#define NSSD_SERVICE_REQ_INITIALIZE(request, fields)            \
  nssd_protocol_packet_fields_initialize(request, fields)

#define NSSD_SERVICE_REQ_FIELD_SET_SCALAR(request, field, field_type, value_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(request, field, field_type, value_type, value)

#define NSSD_SERVICE_REQ_FIELD_SET_UINT32(request, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT32(request, field, field_type, value)

#define NSSD_SERVICE_REQ_FIELD_SET_UINT16(request, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT16(request, field, field_type, value)

#define NSSD_SERVICE_REQ_FIELD_SET_UINT8(request, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT8(request, field, field_type, value)

#define NSSD_SERVICE_REQ_FIELD_SET_ARRAY(request, field, field_type, value, value_length) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_ARRAY(request, field, field_type, value, value_length)

#define NSSD_SERVICE_REQ_FIELD_SET_STRING(request, field, field_type, value, value_length) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_STRING(request, field, field_type, value, value_length)

#define NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request) \
  NSSD_SERVICE_REQ_INITIALIZE(request, 1)

#define NSSD_SERVICE_REQ_SIMPLE_SCALAR(request, field_type, value_type, value) \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_SCALAR(request, 0, field_type, value_type, value); \
  } while(0)

#define NSSD_SERVICE_REQ_SIMPLE_UINT32(request, field_type, value)      \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_UINT32(request, 0, field_type, value);   \
  } while(0)

#define NSSD_SERVICE_REQ_SIMPLE_UINT16(request, field_type, value)      \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_UINT16(request, 0, field_type, value);   \
  } while(0)

#define NSSD_SERVICE_REQ_SIMPLE_UINT8(request, field_type, value)       \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_UINT8(request, 0, field_type, value);    \
  } while(0)

#define NSSD_SERVICE_REQ_SIMPLE_ARRAY(request, field_type, value, value_length) \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_ARRAY(request, 0, field_type, value, value_length); \
  } while(0)

#define NSSD_SERVICE_REQ_SIMPLE_STRING(request, field_type, value, value_length) \
  do {                                                                  \
    NSSD_SERVICE__REQ_SIMPLE_INITIALIZE(request);                       \
    NSSD_SERVICE_REQ_FIELD_SET_STRING(request, 0, field_type, value, value_length); \
  } while(0)

/* Response */

#define NSSD_SERVICE_RESP_NOTFOUND(response_packet)                     \
  do {                                                                  \
    nssd_protocol_packet_fields_initialize(response_packet, 1);         \
                                                                        \
    NSSD_PROTOCOL_PACKET_FIELD_SET_UINT16(response_packet, 0,           \
                                          NSSD_PROTOCOL_RESP_STATUS,    \
                                          NSSD_PROTOCOL_RESPONSE_NOTFOUND); \
  } while(0)

#define NSSD_SERVICE_RESP_FOUND_INITIALIZE(response_packet, fields)     \
  do {                                                                  \
    nssd_protocol_packet_fields_initialize(response_packet, (fields) + 1); \
                                                                        \
    NSSD_PROTOCOL_PACKET_FIELD_SET_UINT16(response_packet, 0,           \
                                          NSSD_PROTOCOL_RESP_STATUS,    \
                                          NSSD_PROTOCOL_RESPONSE_SUCCESS); \
  } while(0)

#define NSSD_SERVICE_RESP_FOUND_FINALIZE(response_packet) /* No-op. */

#define NSSD_SERVICE_RESP_FIELD_SET_SCALAR(response_packet, field, field_type, value_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_SCALAR(response_packet, (field) + 1, field_type, value_type, value)

#define NSSD_SERVICE_RESP_FIELD_SET_UINT32(response_packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT32(response_packet, (field) + 1, field_type, value)

#define NSSD_SERVICE_RESP_FIELD_SET_UINT16(response_packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT16(response_packet, (field) + 1, field_type, value)

#define NSSD_SERVICE_RESP_FIELD_SET_UINT8(response_packet, field, field_type, value) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_UINT8(response_packet, (field) + 1, field_type, value)

#define NSSD_SERVICE_RESP_FIELD_SET_ARRAY(response_packet, field, field_type, value, value_length) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_ARRAY(response_packet, (field) + 1, field_type, value, value_length)

#define NSSD_SERVICE_RESP_FIELD_SET_STRING(response_packet, field, field_type, value, value_length) \
  NSSD_PROTOCOL_PACKET_FIELD_SET_STRING(response_packet, (field) + 1, field_type, value, value_length)

#define NSSD_SERVICE_RESP_FIELD(name)           \
  int (name);

#define NSSD_SERVICE_RESP_ITERATE(response, field) \
  for((field) = 0; (field) < (response)->packet.field_count - 1; (field)++)

#define NSSD_SERVICE_RESP_FIELD_GET(response, field)    \
  (response)->packet.fields[(field) + 1]

#define NSSD_SERVICE_RESP_FIELD_LENGTH(response, field) \
  NSSD_SERVICE_RESP_FIELD_GET(response, field).length

#define NSSD_SERVICE_RESP_FIELD_TYPE(response, field)   \
  NSSD_SERVICE_RESP_FIELD_GET(response, field).type

#define NSSD_SERVICE_RESP_FIELD_GET_SCALAR(response, field, value_type) \
  NSSD_PROTOCOL_PACKET_FIELD_GET_SCALAR(&(response)->packet, field, value_type)

#define NSSD_SERVICE_RESP_FIELD_GET_UINT32(response, field)             \
  NSSD_PROTOCOL_PACKET_FIELD_GET_UINT32(&(response)->packet, (field) + 1)

#define NSSD_SERVICE_RESP_FIELD_GET_UINT16(response, field)          \
  NSSD_PROTOCOL_PACKET_FIELD_GET_UINT16(&(response)->packet, (field) + 1)

#define NSSD_SERVICE_RESP_FIELD_GET_UINT8(response, field)   \
  NSSD_PROTOCOL_PACKET_FIELD_GET_UINT8(&(response)->packet, (field) + 1)

#define NSSD_SERVICE_RESP_FIELD_GET_ARRAY(location, response, field) \
  NSSD_PROTOCOL_PACKET_FIELD_GET_ARRAY(location, &(response)->packet, (field) + 1)

#define NSSD_SERVICE_RESP_FIELD_GET_STRING(location, response, field)   \
  NSSD_PROTOCOL_PACKET_FIELD_GET_STRING(location, &(response)->packet, (field) + 1)

#endif
