/* buffer.h: Mechanism for handling character buffers.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_BUFFER_H
#define _SRVD_BUFFER_H

#include <srvd/srvd.h>

#define SRVD_BUFFER(iterator, buffer) \
  char *(iterator) = (char *)(buffer)

#define SRVD_BUFFER_REF(iterator) \
  (iterator)

#define SRVD_BUFFER_CHECK_OFFSET(iterator, buffer, buffer_size, length) \
  ((size_t)((iterator) - (buffer)) + (length) <= (buffer_size))

#define SRVD_BUFFER_CHECK(iterator, buffer, buffer_size) \
  ((size_t)((iterator) - (buffer)) <= (buffer_size))

#define SRVD_BUFFER_ITERATOR_NEXT(iterator, length) \
  (iterator) += (length)

#define SRVD_BUFFER_ITERATOR_PREV(iterator, length) \
  (iterator) -= (length)

#endif
