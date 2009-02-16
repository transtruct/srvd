/* log.c: Logging routines.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <nssd/log.h>

#include <stdarg.h>
#include <stdio.h>

#define _NSSD_LOG_LINE_LENGTH 1024

static const char *const _nssd_log_severity_map[] = {
  [INFO] = "INFO",
  [NOTICE] = "NOTICE",
  [WARNING] = "WARNING",
  [ERROR] = "ERROR"
};

void nssd_log(nssd_log_severity_t severity, const char *restrict format, ...) {
  char buffer[_NSSD_LOG_LINE_LENGTH];
  va_list arguments;

  va_start(arguments, format);
  vsnprintf(buffer, _NSSD_LOG_LINE_LENGTH, format, arguments);
  va_end(arguments);

  fprintf(stderr, "[%s] %s\n", _nssd_log_severity_map[severity], buffer);
}
