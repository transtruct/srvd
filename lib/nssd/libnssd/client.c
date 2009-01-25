/* client.c: Client connector.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/client.h>

nssd_client_t *nssd_client_allocate(void) {
  nssd_client_t *client = malloc(sizeof(nssd_client_t));
  assert(client);

  return client;
}

void nssd_client_initialize(nssd_client_t *client) {
  assert(client);
}

void nssd_client_connect(nssd_client_t *client) {
  
}

void nssd_client_finalize(nssd_client_t *client) {
  assert(client);
}

void nssd_client_free(nssd_client_t *client) {
  assert(client);

  free(client);
}
