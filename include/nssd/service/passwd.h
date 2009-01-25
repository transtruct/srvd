/* passwd.h: Common utilities for handling username transactions.
 *
 * This file is part of the nssd NSS backend.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _NSSD_SERVICE_PASSWD_H
#define _NSSD_SERVICE_PASSWD_H

#include <nssd/nssd.h>
#include <nssd/protocol.h>

#define NSSD_SERVICE_PASSWD_REQ_NAME ((nssd_protocol_type_t)701)
#define NSSD_SERVICE_PASSWD_REQ_UID ((nssd_protocol_type_t)702)
#define NSSD_SERVICE_PASSWD_REQ_ENTITIES ((nssd_protocol_type_t)703)

#define NSSD_SERVICE_PASSWD_RESP_NAME ((nssd_protocol_type_t)751)
#define NSSD_SERVICE_PASSWD_RESP_UID ((nssd_protocol_type_t)752)
#define NSSD_SERVICE_PASSWD_RESP_GID ((nssd_protocol_type_t)753)
#define NSSD_SERVICE_PASSWD_RESP_DIR ((nssd_protocol_type_t)754)
#define NSSD_SERVICE_PASSWD_RESP_SHELL ((nssd_protocol_type_t)755)

#endif
