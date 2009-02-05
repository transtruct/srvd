/* conf.c: Configuration support.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/conf.h>

#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>

nssd_conf_t *nssd_conf_allocate(void) {
  nssd_conf_t *conf = malloc(sizeof(nssd_conf_t));
  assert(conf);

  return conf;
}

void nssd_conf_initialize(nssd_conf_t *conf) {
  assert(conf);

  conf->nodes = NULL;
}

void nssd_conf_finalize(nssd_conf_t *conf) {
  assert(conf);

  nssd_conf_clear(conf);
}

void nssd_conf_free(nssd_conf_t *conf) {
  assert(conf);

  free(conf);
}

void nssd_conf_clear(nssd_conf_t *conf) {
  assert(conf);

  nssd_conf_node_t *i, *ni;
  for(i = conf->nodes, ni = i ? i->next : NULL;
      i != NULL;
      i = ni, ni = i ? i->next : NULL) {
    free(i->item.name);
    free(i->item.value);
    free(i);
  }
  conf->nodes = NULL;
}

nssd_boolean_t nssd_conf_item_add(nssd_conf_t *conf,
                                const char *name, size_t name_length,
                                const char *value, size_t value_length) {
  assert(conf);

  if(!nssd_conf_item_has(conf, name)) {
    nssd_conf_node_t *node = malloc(sizeof(nssd_conf_node_t));

    node->item.name = malloc(sizeof(char) * name_length);
    strncpy(node->item.name, name, name_length);
    node->item.name[name_length - 1] = '\0';
    node->item.name_length = name_length;

    node->item.value = malloc(sizeof(char) * value_length);
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

nssd_boolean_t nssd_conf_item_get(nssd_conf_t *conf, const char *name, char **value, size_t *value_length) {
  assert(conf);
  assert(value);

  nssd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0) {
      *value = malloc(sizeof(char) * i->item.value_length);
      strncpy(*value, i->item.value, i->item.value_length);

      if(value_length)
        *value_length = i->item.value_length;

      return NSSD_TRUE;
    }
  }

  return NSSD_FALSE;
}

nssd_boolean_t nssd_conf_item_has(nssd_conf_t *conf, const char *name) {
  assert(conf);
  assert(name);

  nssd_conf_node_t *i = conf->nodes;
  for(; i != NULL; i = i->next) {
    if(strncmp(i->item.name, name, i->item.name_length) == 0)
      return NSSD_TRUE;
  }

  return NSSD_FALSE;
}

void nssd_conf_item_free(nssd_conf_t *conf, char **value) {
  assert(conf);
  assert(value);
  assert(*value);

  free(*value);
}

nssd_conf_file_t *nssd_conf_file_allocate(void) {
  nssd_conf_file_t *fconf = malloc(sizeof(nssd_conf_file_t));
  assert(fconf);

  return fconf;
}

void nssd_conf_file_initialize(nssd_conf_file_t *fconf, const char *path, size_t path_length) {
  assert(fconf);

  fconf->path = malloc(sizeof(char) * path_length);
  strncpy(fconf->path, path, path_length);
  fconf->path[path_length - 1] = '\0';

  fconf->updated = NSSD_FALSE;
  fconf->updated_time = (time_t)0;

  nssd_conf_initialize(&fconf->conf);
}

void nssd_conf_file_finalize(nssd_conf_file_t *fconf) {
  assert(fconf);

  free(fconf->path);
  fconf->path = NULL;

  nssd_conf_finalize(&fconf->conf);
  fconf->updated = NSSD_FALSE;
  fconf->updated_time = (time_t)0;
}

void nssd_conf_file_free(nssd_conf_file_t *fconf) {
  assert(fconf);

  free(fconf);
}

nssd_boolean_t nssd_conf_file_uptodate(nssd_conf_file_t *fconf) {
  assert(fconf);

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

static nssd_boolean_t _nssd_conf_file_read(const char *buffer, off_t buffer_size, nssd_conf_file_t *fconf) {
  nssd_boolean_t result = NSSD_TRUE;
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
  for(bp = (char *)buffer; !_BP_EOF_CHECK(bp); ) {
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
    _nssd_conf_file_read_state_assign:
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
        printf("Syntax error: Unexpected comment (expecting '=') at %s:%d\n",
               fconf->path, line);
        _BP_LINE_READ(bp);

        state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
        result = NSSD_FALSE;
        break;
      case '\r':
      case '\n':
        printf("Syntax error: Unexpected newline (expecting '=') at %s:%d\n",
               fconf->path, line);
        _BP_EOL_READ(bp);

        state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
        result = NSSD_FALSE;
        break;
      default:
        printf("Syntax error: Unexpected character '%c' (expecting '=') at %s:%d\n",
               *bp, fconf->path, line);
        bp++;

        result = NSSD_FALSE;
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
          printf("Syntax error: Unexpected end of file (expecting '\"') at %s:%u\n",
                 fconf->path, line);

          state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
          result = NSSD_FALSE;
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
        printf("Syntax error: Unexpected continuation of line (expecting newline) at %s:%u\n",
               fconf->path, line);
        /* Reset us to the end of the line. */
        _BP_LINE_READ(bp);

        result = NSSD_FALSE;
      }

      if(result)
        nssd_conf_item_add(&fconf->conf,
                           name_start, name_end - name_start + 1,
                           value_start, value_end - value_start + 1);

      _CLEAR_VALUES();
      state = _NSSD_CONF_FILE_READ_STATE_GLOBAL;
      break;
    }
  }

  if(state != _NSSD_CONF_FILE_READ_STATE_GLOBAL) {
    printf("Syntax error: Unexpected end of file at %s:%u\n",
           fconf->path, line);

    result = NSSD_FALSE;
  }

  if(!result)
    nssd_conf_clear(&fconf->conf);

  return result;
}

nssd_boolean_t nssd_conf_file_parse(nssd_conf_file_t *fconf) {
  assert(fconf);

  FILE *descriptor = NULL;
  char *buffer = NULL;
  struct stat info;
  nssd_boolean_t result = NSSD_FALSE;

  descriptor = fopen(fconf->path, "rb");
  if(!descriptor) {
    goto _nssd_conf_file_parse_error;
  }

  if(stat(fconf->path, &info) || !info.st_size) {
    goto _nssd_conf_file_parse_error;
  }

  buffer = malloc(sizeof(char) * (info.st_size + 1));
  if(!buffer) {
    goto _nssd_conf_file_parse_error;
  }
  memset(buffer, '\0', sizeof(char) * (info.st_size + 1));

  if(fread(buffer, sizeof(char), info.st_size, descriptor) == -1) {
    goto _nssd_conf_file_parse_error;
  }

  fclose(descriptor);
  descriptor = NULL;

  nssd_conf_clear(&fconf->conf);
  if(_nssd_conf_file_read(buffer, info.st_size + 1, fconf)) {
    fconf->updated = NSSD_TRUE;
    fconf->updated_time = info.st_mtime;

    result = NSSD_TRUE;
  }
  else
    result = NSSD_FALSE;

 _nssd_conf_file_parse_error:

  if(buffer)
    free(buffer);
  if(descriptor)
    fclose(descriptor);

  return result;
}

static nssd_conf_file_t fconf_default = {
  .path = NULL
};

nssd_boolean_t nssd_conf_file_default_get(nssd_conf_file_t **fconfp) {
  assert(fconfp);
  assert(*fconfp == NULL);

  if(fconf_default.path == NULL)
    nssd_conf_file_initialize(&fconf_default,
                              NSSD_CONF_FILE_DEFAULT_PATH, strlen(NSSD_CONF_FILE_DEFAULT_PATH) + 1);

  if(!nssd_conf_file_uptodate(&fconf_default)) {
    if(!nssd_conf_file_parse(&fconf_default))
      return NSSD_FALSE;
  }

  *fconfp = &fconf_default;

  return NSSD_TRUE;
}
