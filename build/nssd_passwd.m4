# nssd_passwd.m4: Automated detection for passwd database support.
#
# This file is part of the nssd NSS backend.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

AC_DEFUN([NSSD_PASSWD], [
AC_LANG(C)

AC_MSG_CHECKING([for pw_gecos in struct passwd])
AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE([[
#include <pwd.h>
const size_t gecos_length = sizeof(((struct passwd *)0)->pw_gecos);]])],
                        [AC_DEFINE([HAVE_PASSWD_GECOS], [1],
                                   [Define to 1 if your `struct passwd' has a pw_gecos field.])
                         AC_MSG_RESULT([yes])],
                        [AC_MSG_RESULT([no])])
])
