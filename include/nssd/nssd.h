/* nssd.h: Basic structures and operations common to all NSS daemon utilities.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_NSSD_H
#define _NSSD_NSSD_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define NSSD_UNUSED(x) (void)(x)

typedef uint8_t nssd_boolean_t;
#define NSSD_TRUE ((nssd_boolean_t)1)
#define NSSD_FALSE ((nssd_boolean_t)0)

#define NSSD__JOIN(x, y) x ## y
#define NSSD_JOIN(x, y) NSSD__JOIN(x, y)

#define NSSD__STRINGIFY(x) #x
#define NSSD_STRINGIFY(x) NSSD__STRINGIFY(x)

#define NSSD_RETURN_IF(condition)              \
  do {                                         \
    if(condition)                              \
      return;                                  \
  } while(0)
#define NSSD_RETURN_VALUE_IF(condition, value) \
  do {                                         \
    if(condition)                              \
      return (value);                          \
  } while(0)
#define NSSD_RETURN_NULL_IF(condition)          \
  NSSD_RETURN_VALUE_IF(condition, NULL)
#define NSSD_RETURN_FALSE_IF(condition)         \
  NSSD_RETURN_VALUE_IF(condition, NSSD_FALSE)
#define NSSD_RETURN_TRUE_IF(condition)          \
  NSSD_RETURN_VALUE_IF(condition, NSSD_TRUE)

#define NSSD_RETURN_UNLESS(condition)           \
  NSSD_RETURN_IF(!(condition))
#define NSSD_RETURN_VALUE_UNLESS(condition, value)      \
  NSSD_RETURN_VALUE_IF(!(condition), value)
#define NSSD_RETURN_NULL_UNLESS(condition)      \
  NSSD_RETURN_NULL_IF(!(condition))
#define NSSD_RETURN_FALSE_UNLESS(condition)     \
  NSSD_RETURN_FALSE_IF(!(condition))
#define NSSD_RETURN_TRUE_UNLESS(condition)      \
  NSSD_RETURN_TRUE_IF(!(condition))

#include <nssd/log.h>

#endif
