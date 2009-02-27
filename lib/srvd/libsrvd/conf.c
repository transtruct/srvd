/* conf.c: Configuration support.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/conf.h>
#include <srvd/thread.h>

#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>

srvd_conf_t *srvd_conf_allocate(void) {
  srvd_conf_t *conf = malloc(sizeof(srvd_conf_t));
  SRVD_RETURN_NULL_UNLESS(conf);

  return conf;
}

void srvd_conf_free(srvd_conf_t *conf) {
  SRVD_RETURN_UNLESS(conf);

  free(conf);
}

srvd_boolean_t srvd_conf_initialize(srvd_conf_t *conf) {
  SRVD_RETURN_FALSE_UNLESS(conf);

  conf->nodes = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_conf_finalize(srvd_conf_t *conf) {
  SRVD_RETURN_FALSE_UNLESS(conf);

  srvd_conf_clear(conf);

  return SRVD_TRUE;
}

srvd_boolean_t srvd_conf_clear(srvd_conf_t *conf) {
  SRVD_RETURN_FALSE_UNLESS(conf);

  srvd_conf_node_t *i, *ni;
  for(i = conf->nodes, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    free(i->item.name);
    free(i->item.value);
    free(i);
  }
  conf->nodes = NULL;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_conf_item_add(srvd_conf_t *conf,
                                  const char *name, size_t name_length,
                                  const char *value, size_t value_length) {
  SRVD_RETURN_FALSE_UNLESS(conf);

  if(!srvd_conf_item_has(conf, name)) {
    srvd_conf_node_t *node = malloc(sizeof(srvd_conf_node_t));
    if(node == NULL) {
      SRVD_LOG_ERROR("srvd_conf_item_add: Unable to allocate memory for list node");
      return SRVD_FALSE;
    }

    node->item.name = malloc(sizeof(char) * name_length);
    if(node->item.name == NULL) {
      SRVD_LOG_ERROR("srvd_conf_item_add: Unable to allocate memory for item name");
      free(node);
      return SRVD_FALSE;
    }

    strncpy(node->item.name, name, name_length);
    node->item.name[name_length - 1] = '\0';
    node->item.name_length = name_length;

    node->item.value = malloc(sizeof(char) * value_length);
    if(node->item.value == NULL) {
      SRVD_LOG_ERROR("srvd_conf_item_add: Unable to allocate memory for item value");
      free(node->item.name);
      free(node);
      return SRVD_FALSE;
    }

    strncpy(node->item.value, value, value_length);
    node->item.value[value_length - 1] = '\0';
    node->item.value_length = value_length;

    node->next = conf->nodes;

    conf->nodes = node;

    return SRVD_TRUE;
  }
  else
    return SRVD_FALSE;
}

srvd_boolean_t srvd_conf_item_get(const srvd_conf_t *conf,
                                  const char *name, char **value, size_t *value_length) {
  SRVD_RETURN_FALSE_UNLESS(conf);
  SRVD_RETURN_FALSE_UNLESS(name);
  SRVD_RETURN_FALSE_UNLESS(*value == NULL);

  srvd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0) {
      *value = i->item.value;

      if(value_length)
        *value_length = i->item.value_length;

      return SRVD_TRUE;
    }
  }

  return SRVD_FALSE;
}

srvd_boolean_t srvd_conf_item_has(const srvd_conf_t *conf, const char *name) {
  SRVD_RETURN_FALSE_UNLESS(conf);
  SRVD_RETURN_FALSE_UNLESS(name);

  srvd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0)
      return SRVD_TRUE;
  }

  return SRVD_FALSE;
}

srvd_conf_file_t *srvd_conf_file_allocate(void) {
  srvd_conf_file_t *fconf = malloc(sizeof(srvd_conf_file_t));
  SRVD_RETURN_NULL_UNLESS(fconf);

  return fconf;
}

void srvd_conf_file_free(srvd_conf_file_t *fconf) {
  SRVD_RETURN_UNLESS(fconf);

  free(fconf);
}

srvd_boolean_t srvd_conf_file_initialize(srvd_conf_file_t *fconf,
                                         const char *path, size_t path_length) {
  SRVD_RETURN_FALSE_UNLESS(fconf);
  SRVD_RETURN_FALSE_UNLESS(path);

  fconf->path = malloc(sizeof(char) * path_length);
  if(fconf->path == NULL) {
    SRVD_LOG_ERROR("srvd_conf_file_initialize: Unable to allocate memory for path");
    return SRVD_FALSE;
  }

  strncpy(fconf->path, path, path_length);
  fconf->path[path_length - 1] = '\0';

  fconf->updated = SRVD_FALSE;
  fconf->updated_time = (time_t)0;

  if(!srvd_conf_initialize(&fconf->conf)) {
    SRVD_LOG_ERROR("srvd_conf_file_initialize: Unable to initialize configuration");
    free(fconf->path);
    return SRVD_FALSE;
  }

  return SRVD_TRUE;
}

srvd_boolean_t srvd_conf_file_finalize(srvd_conf_file_t *fconf) {
  SRVD_RETURN_FALSE_UNLESS(fconf);

  if(fconf->path)
    free(fconf->path);
  fconf->path = NULL;

  if(!srvd_conf_finalize(&fconf->conf)) {
    SRVD_LOG_ERROR("srvd_conf_file_finalize: Unable to finalize configuration");
    return SRVD_FALSE;
  }

  fconf->updated = SRVD_FALSE;
  fconf->updated_time = (time_t)0;

  return SRVD_TRUE;
}

srvd_boolean_t srvd_conf_file_uptodate(const srvd_conf_file_t *fconf) {
  SRVD_RETURN_FALSE_UNLESS(fconf);

  if(!fconf->updated)
    return SRVD_FALSE;

  struct stat info;
  if(stat(fconf->path, &info) == 0 && info.st_mtime) {
    if(info.st_mtime != fconf->updated_time)
      return SRVD_FALSE;
    else
      return SRVD_TRUE;
  }
  else {
    return SRVD_FALSE;
  }
}

enum _srvd_conf_file_read_state {
  _SRVD_CONF_FILE_READ_STATE_GLOBAL,
  _SRVD_CONF_FILE_READ_STATE_NAME,
  _SRVD_CONF_FILE_READ_STATE_ASSIGN,
  _SRVD_CONF_FILE_READ_STATE_VALUE,
  _SRVD_CONF_FILE_READ_STATE_CLOSE
};

/* These parsers are never pretty. I'm a bit sorry about this, really, but
 * it'll have to do for now. */
static srvd_boolean_t _srvd_conf_file_read(char *buffer, size_t buffer_size,
                                           srvd_conf_file_t *fconf) {
  srvd_boolean_t status = SRVD_TRUE;
  enum _srvd_conf_file_read_state state = _SRVD_CONF_FILE_READ_STATE_GLOBAL;

  char *bp;
#define _BP_EOF_CHECK(p) ((p) >= buffer + buffer_size - 1)
#define _BP_EOL_CHECK(p) (*(p) == '\r' || *(p) == '\n')
#define _BP_NAME_CHECK(p) (isalnum(*p) || *(p) == ':')
#define _BP_NAME_READ(p) while((p)++ && !_BP_EOF_CHECK(p) && _BP_NAME_CHECK(p))
#define _BP_SPACE_CHECK(p) (*(p) == ' ' || *(p) == '\t')
#define _BP_SPACE_READ(p) while((p)++ && !_BP_EOF_CHECK(p) && _BP_SPACE_CHECK(p))
#define _BP_COMMENT_READ(p) while((p)++ && !_BP_EOF_CHECK(p) && !_BP_EOL_CHECK(p))
#define _BP_LINE_READ(p) while((p)++ && !_BP_EOF_CHECK(p) && !_BP_EOL_CHECK(p))

  unsigned long line = 1;
#define _BP_EOL_READ(p)                         \
  do {                                          \
    if(*(p) == '\r') {                          \
      if(*((p) + 1) == '\n')                    \
        (p)++;                                  \
      line++;                                   \
    }                                           \
    else if(*(p) == '\n')                       \
      line++;                                   \
    (p)++;                                      \
  } while(0)

  char *name_start, *name_end, *value_start, *value_end;
#define _CLEAR_VALUES() name_start = name_end = value_start = value_end = NULL
  _CLEAR_VALUES();

  /* Parse the buffer. With _BP_EOF_CHECK(), we leave an extra byte at the end,
   * so we can safely read bp and bp + 1. The last byte should be '\0'. */
  for(bp = buffer; !_BP_EOF_CHECK(bp); ) {
    if(*bp == '#')
      _BP_COMMENT_READ(bp);

    switch(state) {
    case _SRVD_CONF_FILE_READ_STATE_GLOBAL:
      switch(*bp) {
      case '\r':
      case '\n':
      case '\t':
      case ' ':
        _BP_EOL_READ(bp);
        break;
      default:
        state = _SRVD_CONF_FILE_READ_STATE_NAME;
        goto _srvd_conf_file_read_state_name;
      }
      break;
    case _SRVD_CONF_FILE_READ_STATE_NAME:
    _srvd_conf_file_read_state_name: 
      name_start = bp;
      _BP_NAME_READ(bp);
      name_end = bp;
      state = _SRVD_CONF_FILE_READ_STATE_ASSIGN;
      /* Fall through. */
    case _SRVD_CONF_FILE_READ_STATE_ASSIGN:
      switch(*bp) {
      case ' ':
      case '\t':
        _BP_SPACE_READ(bp);
        break;
      case '=':
        _BP_SPACE_READ(bp);
        state = _SRVD_CONF_FILE_READ_STATE_VALUE;
        goto _srvd_conf_file_read_state_value;
      case '#':
        SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected comment (expecting '=') "
                       "at %s:%lu\n", fconf->path, line);
        _BP_LINE_READ(bp);

        state = _SRVD_CONF_FILE_READ_STATE_GLOBAL;
        status = SRVD_FALSE;
        break;
      case '\r':
      case '\n':
        SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected newline (expecting '=') "
                       "at %s:%lu\n", fconf->path, line);
        _BP_EOL_READ(bp);

        state = _SRVD_CONF_FILE_READ_STATE_GLOBAL;
        status = SRVD_FALSE;
        break;
      default:
        SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected character '%c' (expecting "
                       "'=') at %s:%lu\n", *bp, fconf->path, line);
        bp++;

        status = SRVD_FALSE;
        break;
      }
      break;
    case _SRVD_CONF_FILE_READ_STATE_VALUE:
    _srvd_conf_file_read_state_value:
      switch(*bp) {
      case ' ':
      case '\t':
        _BP_SPACE_READ(bp);
        break;
      case '\r':
      case '\n':
        state = _SRVD_CONF_FILE_READ_STATE_CLOSE;
        break;
      case '"':
        value_start = bp + 1;
        while(bp++ && !_BP_EOF_CHECK(bp) && *bp != '"') {
          switch(*bp) {
          case '\\':
            if(*(bp + 1) == '"')
              bp++;
            break;
          case '\r':
          case '\n':
            _BP_EOL_READ(bp);
            break;
          }
        }
        if(_BP_EOF_CHECK(bp) && *bp != '"') {
          SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected end of file (expecting "
                         "'\"') at %s:%lu\n", fconf->path, line);

          state = _SRVD_CONF_FILE_READ_STATE_GLOBAL;
          status = SRVD_FALSE;
        }
        else {
          value_end = bp;
          bp++;

          state = _SRVD_CONF_FILE_READ_STATE_CLOSE;
          goto _srvd_conf_file_read_state_close;
        }
        break;
      default:
        value_start = bp;
        while(!_BP_EOF_CHECK(bp) &&
              bp++ && !_BP_SPACE_CHECK(bp) && !_BP_EOL_CHECK(bp));
        value_end = bp;

        state = _SRVD_CONF_FILE_READ_STATE_CLOSE;
        goto _srvd_conf_file_read_state_close;
        break;
      }
      break;
    case _SRVD_CONF_FILE_READ_STATE_CLOSE:
    _srvd_conf_file_read_state_close:
      if(_BP_EOL_CHECK(bp))
        _BP_EOL_READ(bp);
      else if(!_BP_EOF_CHECK(bp)) {
        SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected continuation of line "
                       "(expecting newline) at %s:%lu\n", fconf->path, line);
        /* Reset us to the end of the line. */
        _BP_LINE_READ(bp);

        status = SRVD_FALSE;
      }

      if(status)
        srvd_conf_item_add(&fconf->conf,
                           name_start, (size_t)(name_end - name_start + 1),
                           value_start, (size_t)(value_end - value_start + 1));

      _CLEAR_VALUES();
      state = _SRVD_CONF_FILE_READ_STATE_GLOBAL;
      break;
    }
  }

  if(state != _SRVD_CONF_FILE_READ_STATE_GLOBAL) {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Syntax error: Unexpected end of file at %s:%lu\n",
                   fconf->path, line);

    status = SRVD_FALSE;
  }

  if(status == SRVD_FALSE)
    srvd_conf_clear(&fconf->conf);

  return status;
}

srvd_boolean_t srvd_conf_file_parse(srvd_conf_file_t *fconf) {
  SRVD_RETURN_FALSE_UNLESS(fconf);

  srvd_boolean_t status = SRVD_FALSE;
  FILE *descriptor = NULL;
  char *buffer = NULL;
  size_t buffer_size;
  struct stat info;

  descriptor = fopen(fconf->path, "rb");
  if(!descriptor) {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Unable to open configuration file \"%s\"", fconf->path);
    goto _srvd_conf_file_parse_error;
  }

  if(stat(fconf->path, &info) || !info.st_size) {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Unable to stat() configuration file \"%s\"",
                   fconf->path);
    goto _srvd_conf_file_parse_error;
  }
  buffer_size = (size_t)info.st_size + 1;

  buffer = malloc(sizeof(char) * buffer_size);
  if(buffer == NULL) {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Unable to allocate buffer for configuration file \"%s\"",
                   fconf->path);
    goto _srvd_conf_file_parse_error;
  }
  memset(buffer, '\0', sizeof(char) * buffer_size);

  if(fread(buffer, sizeof(char), buffer_size - 1, descriptor) != sizeof(char) * (buffer_size - 1)) {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Unable to read data from configuration file \"%s\"",
                   fconf->path);
    goto _srvd_conf_file_parse_error;
  }

  fclose(descriptor);
  descriptor = NULL;

  srvd_conf_clear(&fconf->conf);
  if(_srvd_conf_file_read(buffer, buffer_size, fconf)) {
    fconf->updated = SRVD_TRUE;
    fconf->updated_time = info.st_mtime;

    status = SRVD_TRUE;
  }
  else {
    SRVD_LOG_ERROR("srvd_conf_file_parse: Unable to parse configuration file");
    status = SRVD_FALSE;
  }

 _srvd_conf_file_parse_error:

  if(buffer)
    free(buffer);
  if(descriptor)
    fclose(descriptor);

  return status;
}

static srvd_conf_file_t fconf_default = {
  .path = NULL
};

static SRVD_THREAD_MUTEX_DECLARE(_srvd_conf_file_default_lock);

srvd_boolean_t srvd_conf_file_default_get(srvd_conf_file_t **fconfp) {
  srvd_boolean_t status = SRVD_FALSE;

  SRVD_RETURN_FALSE_UNLESS(fconfp);
  SRVD_RETURN_FALSE_UNLESS(*fconfp == NULL);

  SRVD_THREAD_MUTEX_LOCK(_srvd_conf_file_default_lock);

  if(fconf_default.path == NULL)
    srvd_conf_file_initialize(&fconf_default,
                              SRVD_CONF_FILE_DEFAULT_PATH, strlen(SRVD_CONF_FILE_DEFAULT_PATH) + 1);

  if(!srvd_conf_file_uptodate(&fconf_default)) {
    if(!srvd_conf_file_parse(&fconf_default)) {
      SRVD_LOG_ERROR("srvd_conf_file_default_get: Unable to parse default configuration file");
      goto _srvd_conf_file_default_get_error;
    }
  }

  *fconfp = &fconf_default;

  status = SRVD_TRUE;

 _srvd_conf_file_default_get_error:

  SRVD_THREAD_MUTEX_UNLOCK(_srvd_conf_file_default_lock);

  return status;
}
