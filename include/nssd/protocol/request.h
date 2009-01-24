/* request.h: NSS client request protocol.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_REQUEST_H
#define _NSSD_PROTOCOL_REQUEST_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>
#include <nssd/protocol/packet.h>
#include <nssd/protocol/response.h>

void nssd_protocol_request(const nssd_protocol_packet_t *, nssd_protocol_response_t *);

#endif
