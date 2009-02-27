# configure.ac: Autoconf instructions.
#
# This file is part of srvd, a service daemon for POSIX-compliant systems.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(srvd, 0.1.0, srvd@lists.transtruct.org, srvd)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([include/srvd/srvd.h])
AC_CONFIG_HEADER([build/include/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_PROG_CC_C_O

# Checks for libraries.
m4_include([build/acx_pthread.m4])
ACX_PTHREAD

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h sys/socket.h sys/un.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_UID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_INT32_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Options for srvd components.
AC_ARG_ENABLE([pam],
        [AS_HELP_STRING([--enable-pam],
                [enable support for the Pluggable Authentication Module architecture @<:@default=yes@:>@])],
        [],
        [enable_pam=yes])
AC_ARG_ENABLE([nss],
        [AS_HELP_STRING([--enable-nss],
                [enable support for the Name Service Switch architecture @<:@default=yes@:>@])],
        [],
        [enable_nss=yes])

# Checks for NSS databases.
AS_IF([test "x$enable_nss" != "xno"],
        [
                m4_include([build/srvd_nss_check_aliases.m4])
                SRVD_NSS_CHECK_ALIASES

                m4_include([build/srvd_nss_check_passwd.m4])
                SRVD_NSS_CHECK_PASSWD
        ])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([memset socket])

AC_CONFIG_FILES([
        Makefile
        lib/Makefile
        lib/srvd/Makefile
        lib/srvd/libsrvd/Makefile
        lib/srvd/libsrvd/libsrvd.pc
        lib/srvd/nss/Makefile
        lib/srvd/pam/Makefile
        include/Makefile
        etc/Makefile
])

AC_OUTPUT
