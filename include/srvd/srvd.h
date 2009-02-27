/* srvd.h: Basic structures and operations common to all srvd utilities.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SRVD_H
#define _SRVD_SRVD_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define SRVD_UNUSED(x) (void)(x)

typedef uint8_t srvd_boolean_t;
#define SRVD_TRUE ((srvd_boolean_t)1)
#define SRVD_FALSE ((srvd_boolean_t)0)

#define SRVD__JOIN(x, y) x ## y
#define SRVD_JOIN(x, y) SRVD__JOIN(x, y)

#define SRVD__STRINGIFY(x) #x
#define SRVD_STRINGIFY(x) SRVD__STRINGIFY(x)

#define SRVD_RETURN_IF(condition)              \
  do {                                         \
    if(condition)                              \
      return;                                  \
  } while(0)
#define SRVD_RETURN_VALUE_IF(condition, value) \
  do {                                         \
    if(condition)                              \
      return (value);                          \
  } while(0)
#define SRVD_RETURN_NULL_IF(condition)          \
  SRVD_RETURN_VALUE_IF(condition, NULL)
#define SRVD_RETURN_FALSE_IF(condition)         \
  SRVD_RETURN_VALUE_IF(condition, SRVD_FALSE)
#define SRVD_RETURN_TRUE_IF(condition)          \
  SRVD_RETURN_VALUE_IF(condition, SRVD_TRUE)

#define SRVD_RETURN_UNLESS(condition)           \
  SRVD_RETURN_IF(!(condition))
#define SRVD_RETURN_VALUE_UNLESS(condition, value)      \
  SRVD_RETURN_VALUE_IF(!(condition), value)
#define SRVD_RETURN_NULL_UNLESS(condition)      \
  SRVD_RETURN_NULL_IF(!(condition))
#define SRVD_RETURN_FALSE_UNLESS(condition)     \
  SRVD_RETURN_FALSE_IF(!(condition))
#define SRVD_RETURN_TRUE_UNLESS(condition)      \
  SRVD_RETURN_TRUE_IF(!(condition))

#include <srvd/log.h>

#endif
