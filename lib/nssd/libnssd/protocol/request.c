/* request.c: NSS client request protocol.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/request.h>

#include <nssd/client.h>
#include <nssd/client/unsock.h>

void nssd_protocol_request(const nssd_protocol_packet_t *packet, nssd_protocol_response_t *response) {
  assert(packet);
  assert(response);

  nssd_client_t *client = nssd_client_unsock_allocate();
  NSSD_CLIENT_INITIALIZE(client);

  NSSD_CLIENT_CONNECT(client);
  
  NSSD_CLIENT_WRITE(client, packet);
  NSSD_CLIENT_READ(client, &response->packet);

  NSSD_CLIENT_DISCONNECT(client);

  if(response->packet.field_count < 1 || response->packet.fields[0].type != NSSD_PROTOCOL_RESP_STATUS) {
    response->status = NSSD_PROTOCOL_RESPONSE_FAIL;
    return;
  }

  response->status = htons(*(uint16_t *)response->packet.fields[0].data);

  NSSD_CLIENT_FINALIZE(client);
  NSSD_CLIENT_FREE(client);
}
