# srvd_nss_check_aliases.m4: Automated detection for aliases database support.
#
# This file is part of srvd, a service daemon for POSIX-compliant systems.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

AC_DEFUN([SRVD_NSS_CHECK_ALIASES], [
AC_LANG(C)

AC_MSG_CHECKING([for aliases database support])
AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE([[
#include <aliases.h>
const struct aliasent alias_entity;]])],
                        [AC_DEFINE([HAVE_ALIASES], [1],
                                   [Define to 1 if you have support for the aliases database.])
                         AC_MSG_RESULT([yes])],
                        [AC_MSG_RESULT([no])])
])
