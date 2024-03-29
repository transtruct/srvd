/* nss.h: Structures for the NSS component of the library.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef __SRVD_NSS_NSS_H
#define __SRVD_NSS_NSS_H

/* In build/include. */
#include "config.h"

#include <nss.h>

#define SRVD_NSS_FAIL(status_variable, errno_variable, status, errnov, jump) \
  do {                                                                  \
    (status_variable) = (status);                                       \
    *(errno_variable) = (errnov);                                       \
    goto jump;                                                          \
  } while(0)

#define SRVD_NSS_NORECORD(status_variable, jump) \
  do {                                           \
    (status_variable) = NSS_STATUS_NOTFOUND;     \
    goto jump;                                   \
  } while(0)

#define SRVD_NSS_UNAVAIL(status_variable, jump) \
  do {                                          \
    (status_variable) = NSS_STATUS_UNAVAIL;     \
    goto jump;                                  \
  } while(0)

#endif
