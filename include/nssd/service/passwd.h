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

#define NSSD_SERVICE_PASSWD_REQUEST_NAME ((nssd_protocol_type_t)701)
#define NSSD_SERVICE_PASSWD_REQUEST_UID ((nssd_protocol_type_t)702)
#define NSSD_SERVICE_PASSWD_REQUEST_ENTITIES ((nssd_protocol_type_t)703)

nssd_boolean_t nssd_service_passwd_request_name_get(const nssd_service_request_t *, char **);
nssd_boolean_t nssd_service_passwd_request_name_free(const nssd_service_request_t *, char **);
nssd_boolean_t nssd_service_passwd_request_uid_get(const nssd_service_request_t *, uid_t *);
nssd_boolean_t nssd_service_passwd_request_entities_get(const nssd_service_request_t *, int32_t *);

#define NSSD_SERVICE_PASSWD_RESPONSE_NAME ((nssd_protocol_type_t)751)
#define NSSD_SERVICE_PASSWD_RESPONSE_UID ((nssd_protocol_type_t)752)
#define NSSD_SERVICE_PASSWD_RESPONSE_GID ((nssd_protocol_type_t)753)
#define NSSD_SERVICE_PASSWD_RESPONSE_DIR ((nssd_protocol_type_t)754)
#define NSSD_SERVICE_PASSWD_RESPONSE_SHELL ((nssd_protocol_type_t)755)

nssd_boolean_t nssd_service_passwd_response_name_set(nssd_service_response_t *,
                                                     const char *, size_t);
nssd_boolean_t nssd_service_passwd_response_uid_set(nssd_service_response_t *, uid_t);
nssd_boolean_t nssd_service_passwd_response_gid_set(nssd_service_response_t *, gid_t);
nssd_boolean_t nssd_service_passwd_response_dir_set(nssd_service_response_t *,
                                                    const char *, size_t);
nssd_boolean_t nssd_service_passwd_response_shell_set(nssd_service_response_t *,
                                                      const char *, size_t);

#endif
