#! /bin/sh
# autogen.sh: Development script to generate a build system.
#
# This file is part of srvd, a service daemon for POSIX-compliant systems.
# Copyright (c) 2008-2009 Transtruct. All rights reserved.
#
# This file is released under the terms of the LICENSE document included with
# this distribution.

PROGRAM="srvd"

SCRIPT=$( basename $0 )
ERROR=0

error() {
    MESSAGE=$1
    echo "error: ${MESSAGE}"

    ERROR=1
}

hint() {
    MESSAGE=$1
    echo "hint: ${MESSAGE}"
}

status() {
    MESSAGE=$1
    echo "${SCRIPT}: ${MESSAGE}"
}

die_if_error() {
    if [ $ERROR -ne 0 ]; then
        exit 1
    fi
}

( autoconf --version 2>&1 >/dev/null ) || {
    error "The \`autoconf\` program must be installed to compile ${PROGRAM}."
}

( automake --version 2>&1 >/dev/null ) || {
    error "The \`automake\` program must be installed to compile ${PROGRAM}."
}

( aclocal --version 2>&1 >/dev/null ) || {
    error "The \`aclocal\` program must be installed to compile ${PROGRAM}."
    hint "\`aclocal\` is usually provided by \`automake\`. Is your version of \`automake\`"
    hint "up to date?"
}

( libtool --version 2>&1 >/dev/null ) || {
    error "The \`libtool\` program must be installed to compile ${PROGRAM}."
}

( libtoolize --version 2>&1 >/dev/null ) || {
    error "The \`libtoolize\` program must be installed to compile ${PROGRAM}."
    hint "\`libtoolize\` is usually provided by \`libtool\`. Is your version of \`libtool\`"
    hint "up to date?"
}

die_if_error

status "Running libtoolize"
( libtoolize --force --copy ) || {
     error "libtoolize failed"
     exit 1
}

status "Running aclocal"
( aclocal ) || {
    error "aclocal failed"
    exit 1
}

status "Running autoheader"
( autoheader ) || {
    error "autoheader failed"
    exit 1
}

status "Running automake"
( automake --add-missing ) || {
    error "automake failed"
    exit 1
}

status "Running autoconf"
( autoconf ) || {
    error "autoconf failed"
    exit 1
}
