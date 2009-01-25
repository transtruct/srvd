# Makefile.am: Automake instructions.
#
# This file is part of the nss-daemon NSS backend.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

# This is 2 for at least all GNU C Library implementations.
NSSD_NSS_API_VERSION = 2:0:0

lib_LTLIBRARIES = libnss_daemon.la

libnss_daemon_la_CPPFLAGS = -I$(top_srcdir)/include
libnss_daemon_la_LDFLAGS = -module -version-info $(NSSD_NSS_API_VERSION) \
	-L$(top_srcdir)/lib/nssd/libnssd -lnssd

AUTOMAKE_OPTIONS = subdir-objects
libnss_daemon_la_SOURCES = \
	passwd.c
