/* log.h: Logging routines.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_LOG_H
#define _NSSD_LOG_H

#include <nssd/nssd.h>

enum nssd_log_severity {
  INFO,
  NOTICE,
  WARNING,
  ERROR
};

typedef enum nssd_log_severity nssd_log_severity_t;

void nssd_log(nssd_log_severity_t, const char *restrict, ...);

#define NSSD_LOG_INFO(...) nssd_log(INFO, __VA_ARGS__)
#define NSSD_LOG_NOTICE(...) nssd_log(NOTICE, __VA_ARGS__)
#define NSSD_LOG_WARNING(...) nssd_log(WARNING, __VA_ARGS__)
#define NSSD_LOG_ERROR(...) nssd_log(ERROR, __VA_ARGS__)

#endif
