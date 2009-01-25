/* server.h: Server connectivity.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_SERVER_H
#define _NSSD_SERVER_H

#include <nssd/nssd.h>
#include <nssd/protocol/packet.h>

typedef struct nssd_server nssd_server_t;
typedef struct nssd_server_service nssd_server_service_t;

typedef nssd_server_t *(*nssd_server_allocate_pt)(void);
typedef void (*nssd_server_free_pt)(nssd_server_t *);
typedef void (*nssd_server_initialize_pt)(nssd_server_t *);
typedef void (*nssd_server_finalize_pt)(nssd_server_t *);
typedef void (*nssd_server_execute_pt)(nssd_server_t *);

#define NSSD_SERVER_HEADER \
  nssd_server_allocate_pt allocate; \
  nssd_server_free_pt free; \
  nssd_server_initialize_pt initialize; \
  nssd_server_finalize_pt finalize; \
  nssd_server_execute_pt execute; \
  nssd_server_service_t *services; \
  NSSD_BOOLEAN executing

struct nssd_server {
  NSSD_SERVER_HEADER;
};

typedef void (*nssd_server_service_handler_pt)(const nssd_protocol_packet_t *, nssd_protocol_packet_t *);

struct nssd_server_service {
  nssd_protocol_type_t type;
  nssd_server_service_handler_pt handler;
  nssd_server_service_t *next;
};

#define NSSD_SERVER_FREE(sv) (sv)->free((sv))
#define NSSD_SERVER_EXECUTE(sv) (sv)->execute((sv))

void nssd_server_initialize(nssd_server_t *);
void nssd_server_finalize(nssd_server_t *);
void nssd_server_service_add(nssd_server_t *, nssd_protocol_type_t, nssd_server_service_handler_pt);
NSSD_BOOLEAN nssd_server_service_get(nssd_server_t *, nssd_protocol_type_t, nssd_server_service_handler_pt *);
NSSD_BOOLEAN nssd_server_service_has(nssd_server_t *, nssd_protocol_type_t);
void nssd_server_service_remove(nssd_server_t *, nssd_protocol_type_t);

#endif
