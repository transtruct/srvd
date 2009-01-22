/* request.c: NSS client request protocol.
 *
 * This file is part of the Pyn library.
 * Copyright (c) 2008 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/protocol/request.h>

#include <arpa/inet.h>
#include <stdint.h>

void nssd_protocol_request(const nssd_protocol_packet *packet, nssd_protocol_response_t *response) {
  ASSERT_MEMORY(packet);
  ASSERT_MEMORY(response);

  nssd_protocol_packet_serial_t request_packet_s, response_packet_s;
  nssd_client_t client;

  nssd_client_initialize(&client);
  nssd_client_connect(&client);

  nssd_protocol_packet_serial_initialize(&request_packet_s);
  nssd_protocol_packet_serial_serialize(&request_packet_s, packet);

  nssd_client_write(&client, request_packet_s.data, request_packet_s.length);
  nssd_client_read(&client, &response_packet_s.data, &response_packet_s.length);

  nssd_protocol_packet_serial_unserialize(&response_packet_s, response->packet);

  if(response->packet.field_count < 1 || response->packet.fields[0].type != NSSD_PROTOCOL_INTERNAL_STATUS) {
    response->status = NSSD_PROTOCOL_RESPONSE_FAIL;
    return;
  }

  response->status = htons((uint16_t)response->packet.fields[0].data);

  return;
}
