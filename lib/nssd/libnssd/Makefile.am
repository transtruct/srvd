# Makefile.am: Automake instructions.
#
# This file is part of the nssd NSS backend.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

NSSD_COMMON_API_VERSION = 0:0:0

lib_LTLIBRARIES = libnssd.la

libnssd_la_CPPFLAGS = -I$(top_srcdir)/include -D_NSSD_INPUT_SYSCONFDIR=$(sysconfdir)
libnssd_la_CFLAGS = -g
libnssd_la_LDFLAGS = -version-info $(NSSD_COMMON_API_VERSION) \
	-lpthread

AUTOMAKE_OPTIONS = subdir-objects
libnssd_la_SOURCES = \
	client/unsock.c \
	conf.c \
	protocol/packet.c \
	protocol/response.c \
	protocol/request.c \
	protocol/serial_packet.c \
	server.c \
	server/unsock.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnssd.pc
