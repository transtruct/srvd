/* buffer.h: Mechanism for handling NSS buffers.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_BUFFER_H
#define _NSSD_BUFFER_H

#include <nssd/nssd.h>

#define NSSD_BUFFER(iterator, buffer) \
  char *(iterator) = (char *)(buffer)

#define NSSD_BUFFER_REF(iterator) \
  (iterator)

#define NSSD_BUFFER_CHECK_OFFSET(iterator, buffer, buffer_size, length) \
  ((iterator) + (length) - (buffer) >= (buffer_size))

#define NSSD_BUFFER_CHECK(iterator, buffer, buffer_size) \
  ((iterator) - (buffer) >= (buffer_size))

#define NSSD_BUFFER_ITERATOR_NEXT(iterator, length) \
  (iterator) += (length)

#define NSSD_BUFFER_ITERATOR_PREV(iterator, length) \
  (iterator) -= (length)

#endif
