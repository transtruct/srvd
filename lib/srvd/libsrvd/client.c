/* client.c: Client connector.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/client.h>
#include <srvd/client/unsock.h>

srvd_boolean_t srvd_client_get_by_conf(srvd_client_t **client, const srvd_conf_t *conf) {
  srvd_client_t *r = NULL;

  SRVD_RETURN_FALSE_UNLESS(client);
  SRVD_RETURN_FALSE_UNLESS(*client == NULL);
  SRVD_RETURN_FALSE_UNLESS(conf);

  /* Get the adapter. */
  char *adapter = NULL;
  size_t adapter_length;
  if(!srvd_conf_item_get(conf, "client:adapter", &adapter, &adapter_length)) {
    SRVD_LOG_ERROR("srvd_client_get_by_conf: No adapter specified in configuration");
    return SRVD_FALSE;
  }

  /* See which one we should start up.
   *
   * XXX: Move this to a lookup table. */
  if(strncmp(adapter, "unsock", adapter_length) == 0) {
    char *path = NULL;
    size_t path_length;
    if(!srvd_conf_item_get(conf, "client:path", &path, &path_length)) {
      SRVD_LOG_ERROR("srvd_client_get_by_conf: No socket path specified for UNIX domain socket "
                     "adapter");
      return SRVD_FALSE;
    }

    r = srvd_client_unsock_allocate();
    if(r == NULL) {
      SRVD_LOG_ERROR("srvd_client_get_by_conf: Unable to allocate memory for client");
      return SRVD_FALSE;
    }

    if(!srvd_client_unsock_initialize(r, path)) {
      SRVD_LOG_ERROR("srvd_client_get_by_conf: Unable to initialize client");
      free(r);
      return SRVD_FALSE;
    }
  }
  else if(strncmp(adapter, "tcp", adapter_length) == 0) {
    SRVD_LOG_ERROR("srvd_client_get_by_conf: The TCP adapter is not implemented");
    return SRVD_FALSE;
  }
  else {
    SRVD_LOG_ERROR("srvd_client_get_by_conf: Invalid adapter \"%s\" specified", adapter);
    return SRVD_FALSE;
  }

  *client = r;

  return SRVD_TRUE;
}
