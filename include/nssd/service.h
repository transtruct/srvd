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

typedef struct nssd_service_request nssd_service_request_t;

struct nssd_service_request {
  nssd_protocol_packet_t packet;
};

typedef uint16_t nssd_service_response_code_t;

#define NSSD_SERVICE_RESPONSE_SUCCESS ((nssd_service_response_code_t)0)
#define NSSD_SERVICE_RESPONSE_FAIL ((nssd_service_response_code_t)1)
#define NSSD_SERVICE_RESPONSE_NOTFOUND ((nssd_service_response_code_t)2)
#define NSSD_SERVICE_RESPONSE_UNAVAIL ((nssd_service_response_code_t)3)

#define NSSD_SERVICE_RESPONSE_UNKNOWN ((nssd_service_response_code_t)65535)

typedef struct nssd_service_response nssd_service_response_t;

struct nssd_service_response {
  nssd_protocol_packet_t packet;
  nssd_service_response_code_t status;
};

nssd_service_request_t *nssd_service_request_allocate(void);
void nssd_service_request_free(nssd_service_request_t *);
nssd_boolean_t nssd_service_request_initialize(nssd_service_request_t *);
nssd_boolean_t nssd_service_request_finalize(nssd_service_request_t *);

nssd_boolean_t nssd_service_request_query(const nssd_service_request_t *,
                                          nssd_service_response_t *);

/* This is identical to NSSD_PROTOCOL_PACKET_FIELD_ITERATE(), but skips the
 * first field. */
#define NSSD_SERVICE_RESPONSE_FIELD_ITERATE(response, iterator)  \
  for((iterator) = (response)->packet.field_head                 \
        ? (response)->packet.field_head->next                    \
        : NULL;                                                  \
      (iterator) != NULL;                                        \
      (iterator) = (iterator)->next)

nssd_service_response_t *nssd_service_response_allocate(void);
void nssd_service_response_free(nssd_service_response_t *);
nssd_boolean_t nssd_service_response_initialize(nssd_service_response_t *);
nssd_boolean_t nssd_service_response_finalize(nssd_service_response_t *);

#endif
