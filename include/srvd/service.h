/* service.h: Support for services.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SERVICE_H
#define _SRVD_SERVICE_H

#include <srvd/srvd.h>
#include <srvd/protocol.h>
#include <srvd/protocol/packet.h>

typedef struct srvd_service_request srvd_service_request_t;

struct srvd_service_request {
  srvd_protocol_packet_t packet;
};

typedef uint16_t srvd_service_response_code_t;

#define SRVD_SERVICE_RESPONSE_SUCCESS ((srvd_service_response_code_t)0)
#define SRVD_SERVICE_RESPONSE_FAIL ((srvd_service_response_code_t)1)
#define SRVD_SERVICE_RESPONSE_NOTFOUND ((srvd_service_response_code_t)2)
#define SRVD_SERVICE_RESPONSE_UNAVAIL ((srvd_service_response_code_t)3)

#define SRVD_SERVICE_RESPONSE_UNKNOWN ((srvd_service_response_code_t)65535)

typedef struct srvd_service_response srvd_service_response_t;

struct srvd_service_response {
  srvd_protocol_packet_t packet;
  srvd_service_response_code_t status;
};

srvd_service_request_t *srvd_service_request_allocate(void);
void srvd_service_request_free(srvd_service_request_t *);
srvd_boolean_t srvd_service_request_initialize(srvd_service_request_t *);
srvd_boolean_t srvd_service_request_finalize(srvd_service_request_t *);

srvd_boolean_t srvd_service_request_query(const srvd_service_request_t *,
                                          srvd_service_response_t *);

/* This is identical to SRVD_PROTOCOL_PACKET_FIELD_ITERATE(), but skips the
 * first field. */
#define SRVD_SERVICE_RESPONSE_FIELD_ITERATE(response, iterator)  \
  for((iterator) = (response)->packet.field_head                 \
        ? (response)->packet.field_head->next                    \
        : NULL;                                                  \
      (iterator) != NULL;                                        \
      (iterator) = (iterator)->next)

srvd_service_response_t *srvd_service_response_allocate(void);
void srvd_service_response_free(srvd_service_response_t *);
srvd_boolean_t srvd_service_response_initialize(srvd_service_response_t *);
srvd_boolean_t srvd_service_response_finalize(srvd_service_response_t *);

#endif
