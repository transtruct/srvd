/* server.c: Server connectivity.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/server.h>

srvd_boolean_t srvd_server_initialize(srvd_server_t *server) {
  SRVD_RETURN_FALSE_UNLESS(server);

  server->executing = SRVD_FALSE;
  server->services = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_server_finalize(srvd_server_t *server) {
  SRVD_RETURN_FALSE_UNLESS(server);

  server->executing = SRVD_FALSE;

  srvd_server_service_t *i, *ni;
  for(i = server->services, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    free(i);
  }
  server->services = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_server_service_add(srvd_server_t *server, srvd_protocol_type_t type,
                                       srvd_server_service_handler_pt handler) {
  SRVD_RETURN_FALSE_UNLESS(server);

  if(!srvd_server_service_has(server, type)) {
    srvd_server_service_t *node = malloc(sizeof(srvd_server_service_t));
    if(node == NULL) {
      SRVD_LOG_ERROR("srvd_server_service_add: Unable to allocate memory for node");
      return SRVD_FALSE;
    }

    node->type = type;
    node->handler = handler;
    node->next = server->services;

    server->services = node;

    return SRVD_TRUE;
  }
  else
    return SRVD_FALSE;
}

srvd_boolean_t srvd_server_service_get(srvd_server_t *server, srvd_protocol_type_t type, srvd_server_service_handler_pt *handler) {
  SRVD_RETURN_FALSE_UNLESS(server);
  SRVD_RETURN_FALSE_UNLESS(handler);

  srvd_server_service_t *i = server->services;
  for(; i != NULL; i = i->next) {
    if(i->type == type) {
      *handler = i->handler;
      return SRVD_TRUE;
    }
  }

  return SRVD_FALSE;
}

srvd_boolean_t srvd_server_service_has(srvd_server_t *server, srvd_protocol_type_t type) {
  SRVD_RETURN_FALSE_UNLESS(server);

  srvd_server_service_t *i = server->services;
  for(; i != NULL; i = i->next) {
    if(i->type == type)
      return SRVD_TRUE;
  }

  return SRVD_FALSE;
}

srvd_boolean_t srvd_server_service_remove(srvd_server_t *server, srvd_protocol_type_t type) {
  SRVD_RETURN_FALSE_UNLESS(server);

  srvd_server_service_t *pi, *i;
  for(pi = NULL, i = server->services;
      i != NULL;
      pi = i, i = i->next) {
    if(i->type == type) {
      if(pi == NULL)
        server->services = i->next;
      else
        pi->next = i->next;

      free(i);
      return SRVD_TRUE;
    }
  }

  return SRVD_FALSE;
}
