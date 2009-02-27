/* protocol.h: Basic protocol definitions.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_PROTOCOL_H
#define _SRVD_PROTOCOL_H

#include <srvd/srvd.h>

typedef uint16_t srvd_protocol_type_t;

/* No protocol is being used. */
#define SRVD_PROTOCOL_NONE ((srvd_protocol_type_t)0)

/* Reserved for internal use. */
#define SRVD_PROTOCOL_STATUS ((srvd_protocol_type_t)65535)

/* Additional protocol types are defined in the files in the `service'
 * directory and begin with `SRVD_SERVICE_'. */

#endif
