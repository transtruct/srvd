/* nss.h: Structures for the NSS component of the library.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef __NSSD_NSS_NSSD_H
#define __NSSD_NSS_NSSD_H

#include <nss.h>

#define NSSD_NSS_FAIL(status_variable, status, errnov, jump) \
do {                                                         \
  (status_variable) = (status);                              \
  errno = (errnov);                                          \
  goto jump;                                                 \
 } while(0)

#define NSSD_NSS_NORECORD(status_variable, jump) \
  do {                                           \
    (status_variable) = NSS_STATUS_NOTFOUND;     \
    goto jump;                                   \
  } while(0)

#endif
