/* passwd.h: Name switch service for usernames.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef __SRVD_NSS_PASSWD_H
#define __SRVD_NSS_PASSWD_H

#include "nss.h"
#include <srvd/srvd.h>

#include <pwd.h>

enum nss_status _nss_srvd_getpwnam_r(const char *, struct passwd *, char *, size_t, int *);
enum nss_status _nss_srvd_getpwuid_r(uid_t, struct passwd *, char *, size_t, int *);
enum nss_status _nss_srvd_setpwent(int);
enum nss_status _nss_srvd_endpwent(void);
enum nss_status _nss_srvd_getpwent_r(struct passwd *, char *, size_t, int *);

#endif
