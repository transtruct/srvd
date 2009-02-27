/* unsock.h: UNIX socket client.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_CLIENT_UNSOCK_H
#define _SRVD_CLIENT_UNSOCK_H

#include <srvd/srvd.h>
#include <srvd/client.h>

#include <sys/socket.h>
#include <sys/un.h>

typedef struct srvd_client_unsock srvd_client_unsock_t;

struct srvd_client_unsock {
  SRVD_CLIENT_HEADER;
  struct sockaddr_un endpoint;
  int socket;
};

srvd_client_t *srvd_client_unsock_allocate(void);
void srvd_client_unsock_free(srvd_client_t *);
srvd_boolean_t srvd_client_unsock_initialize(srvd_client_t *, const char *);
srvd_boolean_t srvd_client_unsock_finalize(srvd_client_t *);
srvd_boolean_t srvd_client_unsock_connect(srvd_client_t *);
srvd_boolean_t srvd_client_unsock_disconnect(srvd_client_t *);
srvd_boolean_t srvd_client_unsock_write(srvd_client_t *, const srvd_protocol_packet_t *);
srvd_boolean_t srvd_client_unsock_read(srvd_client_t *, srvd_protocol_packet_t *);

#endif
