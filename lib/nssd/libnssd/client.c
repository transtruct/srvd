/* client.c: Client connector.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/client.h>
#include <nssd/client/unsock.h>

nssd_boolean_t nssd_client_get_by_conf(nssd_client_t **client, const nssd_conf_t *conf) {
  nssd_client_t *r = NULL;

  NSSD_RETURN_FALSE_UNLESS(client);
  NSSD_RETURN_FALSE_UNLESS(*client == NULL);
  NSSD_RETURN_FALSE_UNLESS(conf);

  /* Get the adapter. */
  char *adapter = NULL;
  size_t adapter_length;
  if(!nssd_conf_item_get(conf, "client:adapter", &adapter, &adapter_length)) {
    NSSD_LOG_ERROR("nssd_client_get_by_conf: No adapter specified in configuration");
    return NSSD_FALSE;
  }

  /* See which one we should start up.
   *
   * XXX: Move this to a lookup table. */
  if(strncmp(adapter, "unsock", adapter_length) == 0) {
    char *path = NULL;
    size_t path_length;
    if(!nssd_conf_item_get(conf, "client:path", &path, &path_length)) {
      NSSD_LOG_ERROR("nssd_client_get_by_conf: No socket path specified for UNIX domain socket "
                     "adapter");
      return NSSD_FALSE;
    }

    r = nssd_client_unsock_allocate();
    if(r == NULL) {
      NSSD_LOG_ERROR("nssd_client_get_by_conf: Unable to allocate memory for client");
      return NSSD_FALSE;
    }

    if(!nssd_client_unsock_initialize(r, path)) {
      NSSD_LOG_ERROR("nssd_client_get_by_conf: Unable to initialize client");
      free(r);
      return NSSD_FALSE;
    }
  }
  else if(strncmp(adapter, "tcp", adapter_length) == 0) {
    NSSD_LOG_ERROR("nssd_client_get_by_conf: The TCP adapter is not implemented");
    return NSSD_FALSE;
  }
  else {
    NSSD_LOG_ERROR("nssd_client_get_by_conf: Invalid adapter \"%s\" specified", adapter);
    return NSSD_FALSE;
  }

  *client = r;

  return NSSD_TRUE;
}
