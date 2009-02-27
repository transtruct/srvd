# Makefile.am: Automake instructions.
#
# This file is part of srvd, a service daemon for POSIX-compliant systems.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

# This is 2 for at least all GNU C Library implementations.
SRVD_NSS_API_VERSION = 2:0:0

CC = $(PTHREAD_CC)

lib_LTLIBRARIES = libnss_srvd.la

libnss_srvd_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/build/include
libnss_srvd_la_CFLAGS = \
	-pedantic -std=c99 \
	-Wall -W -Wcast-qual -Wcast-align -Winline -Wmissing-prototypes -Wwrite-strings \
	-Wredundant-decls -Wpointer-arith -Wchar-subscripts -Wshadow -Wstrict-prototypes -Werror \
	$(PTHREAD_CFLAGS)
libnss_srvd_la_LDFLAGS = -module -version-info $(SRVD_NSS_API_VERSION) \
	-L$(top_srcdir)/lib/srvd/libsrvd -lsrvd \
	$(PTHREAD_LIBS)

AUTOMAKE_OPTIONS = subdir-objects nostdinc
libnss_srvd_la_SOURCES = \
	passwd.c
