/* server.c: Server connectivity.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/server.h>

void nssd_server_initialize(nssd_server_t *server) {
  assert(server);

  server->executing = NSSD_FALSE;
  server->services = NULL;

  if(server->initialize)
    server->initialize(server);
}

void nssd_server_finalize(nssd_server_t *server) {
  assert(server);

  if(server->finalize)
    server->finalize(server);

  server->executing = NSSD_FALSE;

  nssd_server_service_t *i, *ni;
  for(i = server->services, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    free(i);
  }
  server->services = NULL;
}

void nssd_server_service_add(nssd_server_t *server, nssd_protocol_type_t type, nssd_server_service_handler_pt handler) {
  assert(server);

  if(!nssd_server_service_has(server, type)) {
    nssd_server_service_t *node = malloc(sizeof(nssd_server_service_t));
    node->type = type;
    node->handler = handler;
    node->next = server->services;

    server->services = node;
  }
  else {
    /* Exception! */
  }
}

NSSD_BOOLEAN nssd_server_service_get(nssd_server_t *server, nssd_protocol_type_t type, nssd_server_service_handler_pt *handler) {
  assert(server);
  assert(handler);
  
  nssd_server_service_t *i = server->services;
  for(; i != NULL; i = i->next) {
    if(i->type == type) {
      *handler = i->handler;
      return NSSD_TRUE;
    }
  }

  return NSSD_FALSE;
}

NSSD_BOOLEAN nssd_server_service_has(nssd_server_t *server, nssd_protocol_type_t type) {
  assert(server);

  nssd_server_service_t *i = server->services;
  for(; i != NULL; i = i->next) {
    if(i->type == type)
      return NSSD_TRUE;
  }

  return NSSD_FALSE;
}

void nssd_server_service_remove(nssd_server_t *server, nssd_protocol_type_t type) {
  assert(server);

  nssd_server_service_t *pi, *i;
  for(pi = NULL, i = server->services;
      i != NULL;
      pi = i, i = i->next) {
    if(i->type == type) {
      if(pi == NULL)
        server->services = i->next;
      else
        pi->next = i->next;

      free(i);
      break;
    }
  }
}
