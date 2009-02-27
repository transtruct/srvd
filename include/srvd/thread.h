/* thread.h: Threading support.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_THREAD_H
#define _SRVD_THREAD_H

#include <pthread.h>

#define SRVD_THREAD_KEY_DECLARE(name)           \
  pthread_key_t (name)

#define SRVD_THREAD_KEY_REFERENCE(name)         \
  (pthread_key_t *)(&(name))

#define SRVD_THREAD_KEY_INITIALIZE(name)                                \
  (void)pthread_key_create(SRVD_THREAD_KEY_REFERENCE(name), NULL)

#define SRVD_THREAD_KEY_DATA_GET(name)                  \
  pthread_getspecific(name)

#define SRVD_THREAD_KEY_DATA_HAS(name)          \
  (SRVD_THREAD_KEY_DATA_GET(name) != NULL)

#define SRVD_THREAD_KEY_DATA_SET(name, value)                   \
  pthread_setspecific(name, value)

#define SRVD_THREAD_ONCE_DECLARE(name)                  \
  pthread_once_t (name) = PTHREAD_ONCE_INIT

#define SRVD_THREAD_ONCE_CALL(name, callback)   \
  (void)pthread_once(&(name), callback)

#define SRVD_THREAD_MUTEX_DECLARE(name)                         \
  pthread_mutex_t (name) = PTHREAD_MUTEX_INITIALIZER

#define SRVD_THREAD_MUTEX_DECLARE_UNINITIALIZED(name)   \
  pthread_mutex_t (name)

#define SRVD_THREAD_MUTEX_INITIALIZE(name)      \
  (void)pthread_mutex_init(&(name), NULL)

#define SRVD_THREAD_MUTEX_FINALIZE(name)        \
  (void)pthread_mutex_destroy(&(name))

#define SRVD_THREAD_MUTEX_LOCK(name)            \
  (void)pthread_mutex_lock(&(name))

#define SRVD_THREAD_MUTEX_UNLOCK(name)          \
  (void)pthread_mutex_unlock(&(name))

#endif
