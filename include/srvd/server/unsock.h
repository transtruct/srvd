/* unsock.h: UNIX socket server.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SERVER_UNSOCK_H
#define _SRVD_SERVER_UNSOCK_H

#include <srvd/srvd.h>
#include <srvd/server.h>

#include <sys/socket.h>
#include <sys/un.h>

typedef struct srvd_server_unsock srvd_server_unsock_t;
typedef struct srvd_server_unsock_conf srvd_server_unsock_conf_t;

struct srvd_server_unsock_conf {
  char *path;
  size_t queue_size;
};

struct srvd_server_unsock {
  srvd_server_t monitor;
  srvd_server_unsock_conf_t conf;
  struct sockaddr_un endpoint;
  int socket;
};

srvd_server_unsock_t *srvd_server_unsock_allocate(void);
void srvd_server_unsock_free(srvd_server_unsock_t *);
srvd_boolean_t srvd_server_unsock_initialize(srvd_server_unsock_t *,
                                             const srvd_server_unsock_conf_t *);
srvd_boolean_t srvd_server_unsock_finalize(srvd_server_unsock_t *);
srvd_boolean_t srvd_server_unsock_execute(srvd_server_unsock_t *);

#endif
