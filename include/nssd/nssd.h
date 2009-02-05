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

#include <assert.h>
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

#define _NSSD_JOIN_INTERNAL(x, y) x ## y
#define NSSD_JOIN(x, y) _NSSD_JOIN_INTERNAL(x, y)

#define _NSSD_STRINGIFY_INTERNAL(x) #x
#define NSSD_STRINGIFY(x) _NSSD_STRINGIFY_INTERNAL(x)

#endif
