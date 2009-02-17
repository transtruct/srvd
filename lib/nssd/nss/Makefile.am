# Makefile.am: Automake instructions.
#
# This file is part of the nssd NSS backend.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

# This is 2 for at least all GNU C Library implementations.
NSSD_NSS_API_VERSION = 2:0:0

CC = $(PTHREAD_CC)

lib_LTLIBRARIES = libnss_daemon.la

libnss_daemon_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/build/include
libnss_daemon_la_CFLAGS = \
	-pedantic -std=c99 \
	-Wall -W -Wcast-qual -Wcast-align -Winline -Wmissing-prototypes -Wwrite-strings \
	-Wredundant-decls -Wpointer-arith -Wchar-subscripts -Wshadow -Wstrict-prototypes -Werror \
	$(PTHREAD_CFLAGS)
libnss_daemon_la_LDFLAGS = -module -version-info $(NSSD_NSS_API_VERSION) \
	-L$(top_srcdir)/lib/nssd/libnssd -lnssd \
	$(PTHREAD_LIBS)

AUTOMAKE_OPTIONS = subdir-objects nostdinc
libnss_daemon_la_SOURCES = \
	passwd.c
