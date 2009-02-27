/* log.c: Logging routines.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#include <srvd/log.h>

#include <stdarg.h>
#include <stdio.h>

#define _SRVD_LOG_LINE_LENGTH 1024

static const char *const _srvd_log_severity_map[] = {
  [INFO] = "INFO",
  [NOTICE] = "NOTICE",
  [WARNING] = "WARNING",
  [ERROR] = "ERROR"
};

void srvd_log(srvd_log_severity_t severity, const char *restrict format, ...) {
  char buffer[_SRVD_LOG_LINE_LENGTH];
  va_list arguments;

  va_start(arguments, format);
  vsnprintf(buffer, _SRVD_LOG_LINE_LENGTH, format, arguments);
  va_end(arguments);

  fprintf(stderr, "[%s] %s\n", _srvd_log_severity_map[severity], buffer);
}
