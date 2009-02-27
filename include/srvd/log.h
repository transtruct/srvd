/* log.h: Logging routines.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_LOG_H
#define _SRVD_LOG_H

#include <srvd/srvd.h>

enum srvd_log_severity {
  INFO,
  NOTICE,
  WARNING,
  ERROR
};

typedef enum srvd_log_severity srvd_log_severity_t;

void srvd_log(srvd_log_severity_t, const char *restrict, ...);

#define SRVD_LOG_INFO(...) srvd_log(INFO, __VA_ARGS__)
#define SRVD_LOG_NOTICE(...) srvd_log(NOTICE, __VA_ARGS__)
#define SRVD_LOG_WARNING(...) srvd_log(WARNING, __VA_ARGS__)
#define SRVD_LOG_ERROR(...) srvd_log(ERROR, __VA_ARGS__)

#endif
