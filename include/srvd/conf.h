/* conf.h: Configuration support.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_CONF_H
#define _SRVD_CONF_H

#include <srvd/srvd.h>

#include <time.h>

typedef struct srvd_conf_item srvd_conf_item_t;
typedef struct srvd_conf_node srvd_conf_node_t;
typedef struct srvd_conf srvd_conf_t;
typedef struct srvd_conf_file srvd_conf_file_t;

struct srvd_conf_item {
  size_t name_length, value_length;
  char *name, *value;
};

struct srvd_conf_node {
  srvd_conf_item_t item;
  srvd_conf_node_t *next;
};

struct srvd_conf {
  srvd_conf_node_t *nodes;
};

srvd_conf_t *srvd_conf_allocate(void);
void srvd_conf_free(srvd_conf_t *);
srvd_boolean_t srvd_conf_initialize(srvd_conf_t *);
srvd_boolean_t srvd_conf_finalize(srvd_conf_t *);
srvd_boolean_t srvd_conf_clear(srvd_conf_t *);
srvd_boolean_t srvd_conf_item_add(srvd_conf_t *, const char *, size_t, const char *, size_t);
srvd_boolean_t srvd_conf_item_has(const srvd_conf_t *, const char *);
srvd_boolean_t srvd_conf_item_get(const srvd_conf_t *, const char *, char **, size_t *);

/* File-based configuration. */

struct srvd_conf_file {
  srvd_conf_t conf;
  srvd_boolean_t updated;
  time_t updated_time;
  char *path;
};

srvd_conf_file_t *srvd_conf_file_allocate(void);
void srvd_conf_file_free(srvd_conf_file_t *);
srvd_boolean_t srvd_conf_file_initialize(srvd_conf_file_t *, const char *, size_t);
srvd_boolean_t srvd_conf_file_finalize(srvd_conf_file_t *);
srvd_boolean_t srvd_conf_file_uptodate(const srvd_conf_file_t *);
srvd_boolean_t srvd_conf_file_parse(srvd_conf_file_t *);

/* Default file. */

#define SRVD_CONF_FILE_DEFAULT_PATH \
  SRVD_STRINGIFY(_SRVD_INPUT_SYSCONFDIR) "/srvd.conf"

srvd_boolean_t srvd_conf_file_default_get(srvd_conf_file_t **);

#endif
