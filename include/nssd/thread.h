/* thread.h: Threading support.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_THREAD_H
#define _NSSD_THREAD_H

#include <pthread.h>

#define NSSD_THREAD_KEY_DECLARE(name)           \
  static pthread_key_t name

#define NSSD_THREAD_KEY_REFERENCE(name)         \
  (pthread_key_t *)(&(name))

#define NSSD_THREAD_KEY_INITIALIZE(name)                                \
  (void)pthread_key_create(NSSD_THREAD_KEY_REFERENCE(name), NULL)

#define NSSD_THREAD_KEY_DATA_GET(name)                  \
  pthread_getspecific(name)

#define NSSD_THREAD_KEY_DATA_HAS(name)          \
  (NSSD_THREAD_KEY_DATA_GET(name) != NULL)

#define NSSD_THREAD_KEY_DATA_SET(name, value)                   \
  pthread_setspecific(name, value)

#define NSSD_THREAD_ONCE_DECLARE(name)                  \
  static pthread_once_t name = PTHREAD_ONCE_INIT

#define NSSD_THREAD_ONCE_CALL(name, callback)   \
  (void)pthread_once(&(name), callback)

#endif
