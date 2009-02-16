/* server.h: Server connectivity.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_SERVER_H
#define _NSSD_SERVER_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>
#include <nssd/service.h>

typedef struct nssd_server nssd_server_t;
typedef struct nssd_server_service nssd_server_service_t;

struct nssd_server {
  nssd_server_service_t *services;
  nssd_boolean_t executing;
};

typedef void (*nssd_server_service_handler_pt)(const nssd_service_request_t *, nssd_service_response_t *);

struct nssd_server_service {
  nssd_protocol_type_t type;
  nssd_server_service_handler_pt handler;
  nssd_server_service_t *next;
};

nssd_boolean_t nssd_server_initialize(nssd_server_t *);
nssd_boolean_t nssd_server_finalize(nssd_server_t *);

nssd_boolean_t nssd_server_service_add(nssd_server_t *, nssd_protocol_type_t, nssd_server_service_handler_pt);
nssd_boolean_t nssd_server_service_get(nssd_server_t *, nssd_protocol_type_t, nssd_server_service_handler_pt *);
nssd_boolean_t nssd_server_service_has(nssd_server_t *, nssd_protocol_type_t);
nssd_boolean_t nssd_server_service_remove(nssd_server_t *, nssd_protocol_type_t);

#endif
