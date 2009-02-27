# Makefile.am: Automake instructions.
#
# This file is part of srvd, a service daemon for POSIX-compliant systems.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

SRVD_COMMON_API_VERSION = 0:0:0

CC = $(PTHREAD_CC)

lib_LTLIBRARIES = libsrvd.la

libsrvd_la_CPPFLAGS = -I$(top_srcdir)/include -D_SRVD_INPUT_SYSCONFDIR=$(sysconfdir)
libsrvd_la_CFLAGS = \
	-pedantic -std=c99 \
	-Wall -W -Wcast-qual -Wcast-align -Winline -Wmissing-prototypes -Wwrite-strings \
	-Wredundant-decls -Wpointer-arith -Wchar-subscripts -Wshadow -Wstrict-prototypes -Werror \
	$(PTHREAD_CFLAGS)
libsrvd_la_LDFLAGS = -version-info $(SRVD_COMMON_API_VERSION) \
	$(PTHREAD_LIBS)

AUTOMAKE_OPTIONS = subdir-objects
libsrvd_la_SOURCES = \
	client.c \
	client/unsock.c \
	conf.c \
	log.c \
	protocol/packet.c \
	protocol/serial_packet.c \
	server.c \
	server/unsock.c \
	service.c \
	service/nss/aliases.c \
	service/nss/passwd.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsrvd.pc
