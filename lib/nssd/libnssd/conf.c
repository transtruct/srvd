/* conf.c: Configuration support.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/conf.h>
#include <nssd/thread.h>

#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>

nssd_conf_t *nssd_conf_allocate(void) {
  nssd_conf_t *conf = malloc(sizeof(nssd_conf_t));
  NSSD_RETURN_NULL_UNLESS(conf);

  return conf;
}

void nssd_conf_free(nssd_conf_t *conf) {
  NSSD_RETURN_UNLESS(conf);

  free(conf);
}

nssd_boolean_t nssd_conf_initialize(nssd_conf_t *conf) {
  NSSD_RETURN_FALSE_UNLESS(conf);

  conf->nodes = NULL;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_conf_finalize(nssd_conf_t *conf) {
  NSSD_RETURN_FALSE_UNLESS(conf);

  nssd_conf_clear(conf);

  return NSSD_TRUE;
}

nssd_boolean_t nssd_conf_clear(nssd_conf_t *conf) {
  NSSD_RETURN_FALSE_UNLESS(conf);

  nssd_conf_node_t *i, *ni;
  for(i = conf->nodes, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    free(i->item.name);
    free(i->item.value);
    free(i);
  }
  conf->nodes = NULL;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_conf_item_add(nssd_conf_t *conf,
                                  const char *name, size_t name_length,
                                  const char *value, size_t value_length) {
  NSSD_RETURN_FALSE_UNLESS(conf);

  if(!nssd_conf_item_has(conf, name)) {
    nssd_conf_node_t *node = malloc(sizeof(nssd_conf_node_t));
    if(node == NULL) {
      NSSD_LOG_ERROR("nssd_conf_item_add: Unable to allocate memory for list node");
      return NSSD_FALSE;
    }

    node->item.name = malloc(sizeof(char) * name_length);
    if(node->item.name == NULL) {
      NSSD_LOG_ERROR("nssd_conf_item_add: Unable to allocate memory for item name");
      free(node);
      return NSSD_FALSE;
    }

    strncpy(node->item.name, name, name_length);
    node->item.name[name_length - 1] = '\0';
    node->item.name_length = name_length;

    node->item.value = malloc(sizeof(char) * value_length);
    if(node->item.value == NULL) {
      NSSD_LOG_ERROR("nssd_conf_item_add: Unable to allocate memory for item value");
      free(node->item.name);
      free(node);
      return NSSD_FALSE;
    }

    strncpy(node->item.value, value, value_length);
    node->item.value[value_length - 1] = '\0';
    node->item.value_length = value_length;

    node->next = conf->nodes;

    conf->nodes = node;

    return NSSD_TRUE;
  }
  else
    return NSSD_FALSE;
}

nssd_boolean_t nssd_conf_item_get(const nssd_conf_t *conf,
                                  const char *name, char **value, size_t *value_length) {
  NSSD_RETURN_FALSE_UNLESS(conf);
  NSSD_RETURN_FALSE_UNLESS(name);
  NSSD_RETURN_FALSE_UNLESS(*value == NULL);

  nssd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0) {
      *value = i->item.value;

      if(value_length)
        *value_length = i->item.value_length;

      return NSSD_TRUE;
    }
  }

  return NSSD_FALSE;
}

nssd_boolean_t nssd_conf_item_has(const nssd_conf_t *conf, const char *name) {
  NSSD_RETURN_FALSE_UNLESS(conf);
  NSSD_RETURN_FALSE_UNLESS(name);

  nssd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0)
      return NSSD_TRUE;
  }

  return NSSD_FALSE;
}

nssd_conf_file_t *nssd_conf_file_allocate(void) {
  nssd_conf_file_t *fconf = malloc(sizeof(nssd_conf_file_t));
  NSSD_RETURN_NULL_UNLESS(fconf);

  return fconf;
}

void nssd_conf_file_free(nssd_conf_file_t *fconf) {
  NSSD_RETURN_UNLESS(fconf);

  free(fconf);
}

nssd_boolean_t nssd_conf_file_initialize(nssd_conf_file_t *fconf,
                                         const char *path, size_t path_length) {
  NSSD_RETURN_FALSE_UNLESS(fconf);
  NSSD_RETURN_FALSE_UNLESS(path);

  fconf->path = malloc(sizeof(char) * path_length);
  if(fconf->path == NULL) {
    NSSD_LOG_ERROR("nssd_conf_file_initialize: Unable to allocate memory for path");
    goto _nssd_conf_file_initialize_error;
  }

  strncpy(fconf->path, path, path_length);
  fconf->path[path_length - 1] = '\0';

  fconf->updated = NSSD_FALSE;
  fconf->updated_time = (time_t)0;

  if(!nssd_conf_initialize(&fconf->conf)) {
    NSSD_LOG_ERROR("nssd_conf_file_initialize: Unable to initialize configuration");
    goto _nssd_conf_file_initialize_error;
  }

  return NSSD_TRUE;

 _nssd_conf_file_initialize_error:

  if(fconf->path)
    free(fconf->path);

  return NSSD_FALSE;
}

nssd_boolean_t nssd_conf_file_finalize(nssd_conf_file_t *fconf) {
  NSSD_RETURN_FALSE_UNLESS(fconf);

  if(fconf->path)
    free(fconf->path);
  fconf->path = NULL;

  if(!nssd_conf_finalize(&fconf->conf)) {
    NSSD_LOG_ERROR("nssd_conf_file_finalize: Unable to finalize configuration");
    return NSSD_FALSE;
  }

  fconf->updated = NSSD_FALSE;
  fconf->updated_time = (time_t)0;

  return NSSD_TRUE;
}

nssd_boolean_t nssd_conf_file_uptodate(nssd_conf_file_t *fconf) {
  NSSD_RETURN_FALSE_UNLESS(fconf);

  if(!fconf->updated)
    return NSSD_FALSE;

  struct stat info;
  if(stat(fconf->path, &info) == 0 && info.st_mtime) {
    if(info.st_mtime != fconf->updated_time)
      return NSSD_FALSE;
    else
      return NSSD_TRUE;
  }
  else {
    return NSSD_FALSE;
  }
}

enum _nssd_conf_file_read_state {
  _NSSD_CONF_FILE_READ_STATE_GLOBAL,
  _NSSD_CONF_FILE_READ_STATE_NAME,
  _NSSD_CONF_FILE_READ_STATE_ASSIGN,
  _NSSD_CONF_FILE_READ_STATE_VALUE,
  _NSSD_CONF_FILE_READ_STATE_CLOSE
};

/* These parsers are never pretty. I'm a bit sorry about this, really, but
 * it'll have to do for now. */
static nssd_boolean_t _nssd_conf_file_read(char *buffer, size_t buffer_size,
                                           nssd_conf_file_t *fconf) {
  nssd_boolean_t status = NSSD_TRUE;
  enum _nssd_conf_file_read_state state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;

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
    case _NSSD_CONF_FILE_READ_STATE_GLOBAL:
      switch(*bp) {
      case '\r':
      case '\n':
      case '\t':
      case ' ':
        _BP_EOL_READ(bp);
        break;
      default:
        state = _NSSD_CONF_FILE_READ_STATE_NAME;
        goto _nssd_conf_file_read_state_name;
      }
      break;
    case _NSSD_CONF_FILE_READ_STATE_NAME:
    _nssd_conf_file_read_state_name: 
      name_start = bp;
      _BP_NAME_READ(bp);
      name_end = bp;
      state = _NSSD_CONF_FILE_READ_STATE_ASSIGN;
      /* Fall through. */
    case _NSSD_CONF_FILE_READ_STATE_ASSIGN:
      switch(*bp) {
      case ' ':
      case '\t':
        _BP_SPACE_READ(bp);
        break;
      case '=':
        _BP_SPACE_READ(bp);
        state = _NSSD_CONF_FILE_READ_STATE_VALUE;
        goto _nssd_conf_file_read_state_value;
      case '#':
        NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected comment (expecting '=') "
                       "at %s:%lu\n", fconf->path, line);
        _BP_LINE_READ(bp);

        state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
        status = NSSD_FALSE;
        break;
      case '\r':
      case '\n':
        NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected newline (expecting '=') "
                       "at %s:%lu\n", fconf->path, line);
        _BP_EOL_READ(bp);

        state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
        status = NSSD_FALSE;
        break;
      default:
        NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected character '%c' (expecting "
                       "'=') at %s:%lu\n", *bp, fconf->path, line);
        bp++;

        status = NSSD_FALSE;
        break;
      }
      break;
    case _NSSD_CONF_FILE_READ_STATE_VALUE:
    _nssd_conf_file_read_state_value:
      switch(*bp) {
      case ' ':
      case '\t':
        _BP_SPACE_READ(bp);
        break;
      case '\r':
      case '\n':
        state = _NSSD_CONF_FILE_READ_STATE_CLOSE;
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
          NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected end of file (expecting "
                         "'\"') at %s:%lu\n", fconf->path, line);

          state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
          status = NSSD_FALSE;
        }
        else {
          value_end = bp;
          bp++;

          state = _NSSD_CONF_FILE_READ_STATE_CLOSE;
          goto _nssd_conf_file_read_state_close;
        }
        break;
      default:
        value_start = bp;
        while(!_BP_EOF_CHECK(bp) &&
              bp++ && !_BP_SPACE_CHECK(bp) && !_BP_EOL_CHECK(bp));
        value_end = bp;

        state = _NSSD_CONF_FILE_READ_STATE_CLOSE;
        goto _nssd_conf_file_read_state_close;
        break;
      }
      break;
    case _NSSD_CONF_FILE_READ_STATE_CLOSE:
    _nssd_conf_file_read_state_close:
      if(_BP_EOL_CHECK(bp))
        _BP_EOL_READ(bp);
      else if(!_BP_EOF_CHECK(bp)) {
        NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected continuation of line "
                       "(expecting newline) at %s:%lu\n", fconf->path, line);
        /* Reset us to the end of the line. */
        _BP_LINE_READ(bp);

        status = NSSD_FALSE;
      }

      if(status)
        nssd_conf_item_add(&fconf->conf,
                           name_start, (size_t)(name_end - name_start + 1),
                           value_start, (size_t)(value_end - value_start + 1));

      _CLEAR_VALUES();
      state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
      break;
    }
  }

  if(state != _NSSD_CONF_FILE_READ_STATE_GLOBAL) {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Syntax error: Unexpected end of file at %s:%lu\n",
                   fconf->path, line);

    status = NSSD_FALSE;
  }

  if(status == NSSD_FALSE)
    nssd_conf_clear(&fconf->conf);

  return status;
}

nssd_boolean_t nssd_conf_file_parse(nssd_conf_file_t *fconf) {
  NSSD_RETURN_FALSE_UNLESS(fconf);

  nssd_boolean_t status = NSSD_FALSE;
  FILE *descriptor = NULL;
  char *buffer = NULL;
  size_t buffer_size;
  struct stat info;

  descriptor = fopen(fconf->path, "rb");
  if(!descriptor) {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Unable to open configuration file \"%s\"", fconf->path);
    goto _nssd_conf_file_parse_error;
  }

  if(stat(fconf->path, &info) || !info.st_size) {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Unable to stat() configuration file \"%s\"",
                   fconf->path);
    goto _nssd_conf_file_parse_error;
  }
  buffer_size = (size_t)info.st_size + 1;

  buffer = malloc(sizeof(char) * buffer_size);
  if(buffer == NULL) {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Unable to allocate buffer for configuration file \"%s\"",
                   fconf->path);
    goto _nssd_conf_file_parse_error;
  }
  memset(buffer, '\0', sizeof(char) * buffer_size);

  if(fread(buffer, sizeof(char), buffer_size - 1, descriptor) != sizeof(char) * (buffer_size - 1)) {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Unable to read data from configuration file \"%s\"",
                   fconf->path);
    goto _nssd_conf_file_parse_error;
  }

  fclose(descriptor);
  descriptor = NULL;

  nssd_conf_clear(&fconf->conf);
  if(_nssd_conf_file_read(buffer, buffer_size, fconf)) {
    fconf->updated = NSSD_TRUE;
    fconf->updated_time = info.st_mtime;

    status = NSSD_TRUE;
  }
  else {
    NSSD_LOG_ERROR("nssd_conf_file_parse: Unable to parse configuration file");
    status = NSSD_FALSE;
  }

 _nssd_conf_file_parse_error:

  if(buffer)
    free(buffer);
  if(descriptor)
    fclose(descriptor);

  return status;
}

static nssd_conf_file_t fconf_default = {
  .path = NULL
};

static NSSD_THREAD_MUTEX_DECLARE(_nssd_conf_file_default_lock);

nssd_boolean_t nssd_conf_file_default_get(nssd_conf_file_t **fconfp) {
  nssd_boolean_t status = NSSD_FALSE;

  NSSD_RETURN_FALSE_UNLESS(fconfp);
  NSSD_RETURN_FALSE_UNLESS(*fconfp == NULL);

  NSSD_THREAD_MUTEX_LOCK(_nssd_conf_file_default_lock);

  if(fconf_default.path == NULL)
    nssd_conf_file_initialize(&fconf_default,
                              NSSD_CONF_FILE_DEFAULT_PATH, strlen(NSSD_CONF_FILE_DEFAULT_PATH) + 1);

  if(!nssd_conf_file_uptodate(&fconf_default)) {
    if(!nssd_conf_file_parse(&fconf_default)) {
      NSSD_LOG_ERROR("nssd_conf_file_default_get: Unable to parse default configuration file");
      goto _nssd_conf_file_default_get_error;
    }
  }

  *fconfp = &fconf_default;

  status = NSSD_TRUE;

 _nssd_conf_file_default_get_error:

  NSSD_THREAD_MUTEX_UNLOCK(_nssd_conf_file_default_lock);

  return status;
}
