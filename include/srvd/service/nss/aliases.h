/* aliases.h: Support for the aliases database.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SERVICE_ALIASES_H
#define _SRVD_SERVICE_ALIASES_H

#include <srvd/srvd.h>
#include <srvd/protocol.h>
#include <srvd/service.h>

#define SRVD_SERVICE_NSS_ALIASES_REQUEST_NAME ((srvd_protocol_type_t)101)
#define SRVD_SERVICE_NSS_ALIASES_REQUEST_ENTITIES ((srvd_protocol_type_t)102)

srvd_boolean_t srvd_service_nss_aliases_request_name_get(const srvd_service_request_t *,
                                                         char **);
srvd_boolean_t srvd_service_nss_aliases_request_name_free(const srvd_service_request_t *,
                                                          char **);
srvd_boolean_t srvd_service_nss_aliases_request_entities_get(const srvd_service_request_t *,
                                                             int32_t *);

#define SRVD_SERVICE_NSS_ALIASES_RESPONSE_NAME ((srvd_protocol_type_t)151)
#define SRVD_SERVICE_NSS_ALIASES_RESPONSE_MEMBER ((srvd_protocol_type_t)152)
#define SRVD_SERVICE_NSS_ALIASES_RESPONSE_LOCAL ((srvd_protocol_type_t)153)

srvd_boolean_t srvd_service_nss_aliases_response_name_set(srvd_service_response_t *,
                                                          const char *, size_t);
srvd_boolean_t srvd_service_nss_aliases_response_member_add(srvd_service_response_t *,
                                                            const char *, size_t);
srvd_boolean_t srvd_service_nss_aliases_response_local_set(srvd_service_response_t *,
                                                           srvd_boolean_t);

#endif
