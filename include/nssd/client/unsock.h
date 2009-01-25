/* unsock.h: UNIX socket client.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_CLIENT_UNSOCK_H
#define _NSSD_CLIENT_UNSOCK_H

#include <nssd/nssd.h>
#include <nssd/client.h>

#include <sys/socket.h>
#include <sys/un.h>

typedef struct nssd_client_unsock nssd_client_unsock_t;

struct nssd_client_unsock {
  NSSD_CLIENT_HEADER;
  struct sockaddr_un endpoint;
  int socket;
};

nssd_client_t *nssd_client_unsock_allocate(void);
void nssd_client_unsock_free(nssd_client_t *);
void nssd_client_unsock_initialize(nssd_client_t *);
void nssd_client_unsock_finalize(nssd_client_t *);
void nssd_client_unsock_connect(nssd_client_t *);
void nssd_client_unsock_disconnect(nssd_client_t *);
void nssd_client_unsock_write(nssd_client_t *, const nssd_protocol_packet_t *);
void nssd_client_unsock_read(nssd_client_t *, nssd_protocol_packet_t *);

#endif
