# configure.ac: Autoconf instructions.
#
# This file is part of the nss-daemon NSS backend.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(nss-daemon, 0.1.0, nss-daemon@lists.transtruct.org, nss-daemon)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([include/nssd/nssd.h])
AC_CONFIG_HEADER([build/include/config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h sys/socket.h sys/un.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([
        Makefile
        lib/Makefile
        lib/nssd/Makefile
        lib/nssd/common/Makefile
        lib/nssd/nss/Makefile
        include/Makefile
])

AC_OUTPUT
