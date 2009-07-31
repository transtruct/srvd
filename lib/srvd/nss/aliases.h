/* aliases.h: Name switch service for mail aliases.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef __SRVD_NSS_ALIASES_H
#define __SRVD_NSS_ALIASES_H

#include "nss.h"
#include <srvd/srvd.h>

#ifdef HAVE_ALIASES
#include <aliases.h>

enum nss_status _nss_srvd_getaliasbyname_r(const char *, struct aliasent *, char *, size_t, int *);
enum nss_status _nss_srvd_setaliasent(int);
enum nss_status _nss_srvd_endaliasent(void);
enum nss_status _nss_srvd_getaliasent_r(struct aliasent *, char *, size_t, int *);
#endif

#endif
