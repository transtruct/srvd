/* passwd.h: Name switch service for usernames.
 *
 * This file is part of the nss-daemon NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef __NSSD_NSS_PASSWD_H
#define __NSSD_NSS_PASSWD_H

#include "nssd.h"
#include <nssd/nssd.h>

#include <pwd.h>

enum nss_status _nss_daemon_getpwnam_r(const char *, struct passwd *, char *, size_t, int *);
enum nss_status _nss_daemon_getpwuid_r(uid_t, struct passwd *, char *, size_t, int *);
enum nss_status _nss_daemon_setpwent(int);
enum nss_status _nss_daemon_endpwent(void);
enum nss_status _nss_daemon_getpwent_r(struct passwd *, char *, size_t, int *);

#endif
