/* response.h: NSS client response information.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_RESPONSE_H
#define _NSSD_PROTOCOL_RESPONSE_H

#include <nssd/nssd.h>
#include <nssd/protocol/packet.h>

typedef struct nssd_protocol_response nssd_protocol_response_t;
typedef uint16_t nssd_protocol_response_code_t;

struct nssd_protocol_response {
  nssd_protocol_response_code_t status;
  nssd_protocol_packet_t packet;
};

#define NSSD_PROTOCOL_RESPONSE_SUCCESS ((nssd_protocol_response_code_t)0)
#define NSSD_PROTOCOL_RESPONSE_FAIL ((nssd_protocol_response_code_t)1)
#define NSSD_PROTOCOL_RESPONSE_NOTFOUND ((nssd_protocol_response_code_t)2)

#define NSSD_PROTOCOL_RESPONSE_UNKNOWN ((nssd_protocol_response_code_t)65535)

nssd_protocol_response_t *nssd_protocol_response_allocate(void);
void nssd_protocol_response_initialize(nssd_protocol_response_t *);
void nssd_protocol_response_finalize(nssd_protocol_response_t *);
void nssd_protocol_response_free(nssd_protocol_response_t *);

#endif
