/* unsock.h: UNIX socket server.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_SERVER_UNSOCK_H
#define _NSSD_SERVER_UNSOCK_H

#include <nssd/nssd.h>
#include <nssd/server.h>

#include <sys/socket.h>
#include <sys/un.h>

typedef struct nssd_server_unsock nssd_server_unsock_t;

struct nssd_server_unsock {
  NSSD_SERVER_HEADER;
  struct sockaddr_un endpoint;
  int socket;
};

nssd_server_t *nssd_server_unsock_allocate(void);
void nssd_server_unsock_free(nssd_server_t *);
void nssd_server_unsock_initialize(nssd_server_t *);
void nssd_server_unsock_finalize(nssd_server_t *sv);
void nssd_server_unsock_execute(nssd_server_t *sv);

#endif
