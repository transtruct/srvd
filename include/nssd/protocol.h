/* protocol.h: Basic protocol definitions.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_PROTOCOL_H
#define _NSSD_PROTOCOL_H

#include <nssd/nssd.h>

typedef uint16_t nssd_protocol_type_t;

/* No NSS protocol is being used. */
#define NSSD_PROTOCOL_NONE ((nssd_protocol_type_t)0)

/* Reserved for internal use. */
#define NSSD_PROTOCOL_STATUS ((nssd_protocol_type_t)65535)

/* Additional protocol types are defined in the files in the `service'
 * directory and begin with `NSSD_SERVICE_'. */

#endif
