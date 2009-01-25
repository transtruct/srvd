/* response.c: NSS client response information.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/response.h>

nssd_protocol_response_t *nssd_protocol_response_allocate(void) {
  nssd_protocol_response_t *response = malloc(sizeof(nssd_protocol_response_t));
  assert(response);

  return response;
}

void nssd_protocol_response_initialize(nssd_protocol_response_t *response) {
  assert(response);

  response->status = NSSD_PROTOCOL_RESPONSE_UNKNOWN;
  nssd_protocol_packet_initialize(&response->packet);
}

void nssd_protocol_response_finalize(nssd_protocol_response_t *response) {
  assert(response);

  nssd_protocol_packet_finalize(&response->packet);
}

void nssd_protocol_response_free(nssd_protocol_response_t *response) {
  assert(response);

  free(response);
}
