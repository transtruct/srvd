/* server.h: Server connectivity.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SERVER_H
#define _SRVD_SERVER_H

#include <srvd/srvd.h>
#include <srvd/protocol.h>
#include <srvd/service.h>

typedef struct srvd_server srvd_server_t;
typedef struct srvd_server_service srvd_server_service_t;

struct srvd_server {
  srvd_server_service_t *services;
  srvd_boolean_t executing;
};

typedef void (*srvd_server_service_handler_pt)(const srvd_service_request_t *, srvd_service_response_t *);

struct srvd_server_service {
  srvd_protocol_type_t type;
  srvd_server_service_handler_pt handler;
  srvd_server_service_t *next;
};

srvd_boolean_t srvd_server_initialize(srvd_server_t *);
srvd_boolean_t srvd_server_finalize(srvd_server_t *);

srvd_boolean_t srvd_server_service_add(srvd_server_t *, srvd_protocol_type_t, srvd_server_service_handler_pt);
srvd_boolean_t srvd_server_service_get(srvd_server_t *, srvd_protocol_type_t, srvd_server_service_handler_pt *);
srvd_boolean_t srvd_server_service_has(srvd_server_t *, srvd_protocol_type_t);
srvd_boolean_t srvd_server_service_remove(srvd_server_t *, srvd_protocol_type_t);

#endif
