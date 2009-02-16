/* conf.h: Configuration support.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_CONF_H
#define _NSSD_CONF_H

#include <nssd/nssd.h>

#include <time.h>

typedef struct nssd_conf_item nssd_conf_item_t;
typedef struct nssd_conf_node nssd_conf_node_t;
typedef struct nssd_conf nssd_conf_t;
typedef struct nssd_conf_file nssd_conf_file_t;

struct nssd_conf_item {
  size_t name_length, value_length;
  char *name, *value;
};

struct nssd_conf_node {
  nssd_conf_item_t item;
  nssd_conf_node_t *next;
};

struct nssd_conf {
  nssd_conf_node_t *nodes;
};

nssd_conf_t *nssd_conf_allocate(void);
void nssd_conf_free(nssd_conf_t *);
nssd_boolean_t nssd_conf_initialize(nssd_conf_t *);
nssd_boolean_t nssd_conf_finalize(nssd_conf_t *);
nssd_boolean_t nssd_conf_clear(nssd_conf_t *);
nssd_boolean_t nssd_conf_item_add(nssd_conf_t *, const char *, size_t, const char *, size_t);
nssd_boolean_t nssd_conf_item_has(const nssd_conf_t *, const char *);
nssd_boolean_t nssd_conf_item_get(const nssd_conf_t *, const char *, char **, size_t *);

/* File-based configuration. */

struct nssd_conf_file {
  nssd_conf_t conf;
  nssd_boolean_t updated;
  time_t updated_time;
  char *path;
};

nssd_conf_file_t *nssd_conf_file_allocate(void);
void nssd_conf_file_free(nssd_conf_file_t *);
nssd_boolean_t nssd_conf_file_initialize(nssd_conf_file_t *, const char *, size_t);
nssd_boolean_t nssd_conf_file_finalize(nssd_conf_file_t *);
nssd_boolean_t nssd_conf_file_uptodate(nssd_conf_file_t *);
nssd_boolean_t nssd_conf_file_parse(nssd_conf_file_t *);
nssd_boolean_t nssd_conf_file_get(nssd_conf_file_t *);

/* Default file. */

#define NSSD_CONF_FILE_DEFAULT_PATH \
  NSSD_STRINGIFY(_NSSD_INPUT_SYSCONFDIR) "/nssd.conf"

nssd_boolean_t nssd_conf_file_default_get(nssd_conf_file_t **);

#endif
