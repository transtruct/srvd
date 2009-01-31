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
#include <nssd/service.h>

#define NSSD_SERVICE_PASSWD_REQ_NAME ((nssd_protocol_type_t)701)
#define NSSD_SERVICE_PASSWD_REQ_UID ((nssd_protocol_type_t)702)
#define NSSD_SERVICE_PASSWD_REQ_ENTITIES ((nssd_protocol_type_t)703)

#define NSSD_SERVICE_PASSWD_RESP_NAME ((nssd_protocol_type_t)751)
#define NSSD_SERVICE_PASSWD_RESP_UID ((nssd_protocol_type_t)752)
#define NSSD_SERVICE_PASSWD_RESP_GID ((nssd_protocol_type_t)753)
#define NSSD_SERVICE_PASSWD_RESP_DIR ((nssd_protocol_type_t)754)
#define NSSD_SERVICE_PASSWD_RESP_SHELL ((nssd_protocol_type_t)755)

/* Response */

#define NSSD_SERVICE_PASSWD_RESP_FOUND_INITIALIZE(response) \
  NSSD_SERVICE_RESP_FOUND_INITIALIZE((response), 5)

#define NSSD_SERVICE_PASSWD_RESP_FOUND_FINALIZE(response) \
  NSSD_SERVICE_RESP_FOUND_FINALIZE((response))

#define NSSD_SERVICE_PASSWD_RESP_FOUND_NAME_SET(response, value, value_length) \
  NSSD_SERVICE_RESP_FIELD_SET_STRING((response), 0,                     \
                                     NSSD_SERVICE_PASSWD_RESP_NAME,     \
                                     (value), (value_length))

#define NSSD_SERVICE_PASSWD_RESP_FOUND_UID_SET(response, value)         \
  NSSD_SERVICE_RESP_FIELD_SET_UINT32((response), 1,                     \
                                     NSSD_SERVICE_PASSWD_RESP_UID,      \
                                     (value))

#define NSSD_SERVICE_PASSWD_RESP_FOUND_GID_SET(response, value)         \
  NSSD_SERVICE_RESP_FIELD_SET_UINT32(response, 2,                       \
                                     NSSD_SERVICE_PASSWD_RESP_GID,      \
                                     (value))

#define NSSD_SERVICE_PASSWD_RESP_FOUND_DIR_SET(response, value, value_length) \
  NSSD_SERVICE_RESP_FIELD_SET_STRING(response, 3,                       \
                                     NSSD_SERVICE_PASSWD_RESP_DIR,      \
                                     (value), (value_length))

#define NSSD_SERVICE_PASSWD_RESP_FOUND_SHELL_SET(response, value, value_length) \
  NSSD_SERVICE_RESP_FIELD_SET_STRING(response, 4,                       \
                                     NSSD_SERVICE_PASSWD_RESP_SHELL,    \
                                     (value), (value_length));

#endif
