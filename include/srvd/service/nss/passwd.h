/* passwd.h: Support for the passwd database.
 *
 * This file is part of srvd, a service daemon for POSIX-compliant systems.
 * Copyright (c) 2008-2009 Transtruct. All rights reserved.
 *
 * This file is released under the terms of the LICENSE document included with
 * this distribution.
 */

#ifndef _SRVD_SERVICE_NSS_PASSWD_H
#define _SRVD_SERVICE_NSS_PASSWD_H

#include <srvd/srvd.h>
#include <srvd/protocol.h>
#include <srvd/service.h>

#define SRVD_SERVICE_NSS_PASSWD_REQUEST_NAME ((srvd_protocol_type_t)701)
#define SRVD_SERVICE_NSS_PASSWD_REQUEST_UID ((srvd_protocol_type_t)702)
#define SRVD_SERVICE_NSS_PASSWD_REQUEST_ENTITIES ((srvd_protocol_type_t)703)

srvd_boolean_t srvd_service_nss_passwd_request_name_get(const srvd_service_request_t *,
                                                        char **);
srvd_boolean_t srvd_service_nss_passwd_request_name_free(const srvd_service_request_t *,
                                                         char **);
srvd_boolean_t srvd_service_nss_passwd_request_uid_get(const srvd_service_request_t *,
                                                       uid_t *);
srvd_boolean_t srvd_service_nss_passwd_request_entities_get(const srvd_service_request_t *,
                                                            int32_t *);

#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_NAME ((srvd_protocol_type_t)751)
#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_UID ((srvd_protocol_type_t)752)
#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_GID ((srvd_protocol_type_t)753)
#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_DIR ((srvd_protocol_type_t)754)
#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_SHELL ((srvd_protocol_type_t)755)
#define SRVD_SERVICE_NSS_PASSWD_RESPONSE_GECOS ((srvd_protocol_type_t)756)

srvd_boolean_t srvd_service_nss_passwd_response_name_set(srvd_service_response_t *,
                                                         const char *, size_t);
srvd_boolean_t srvd_service_nss_passwd_response_uid_set(srvd_service_response_t *, uid_t);
srvd_boolean_t srvd_service_nss_passwd_response_gid_set(srvd_service_response_t *, gid_t);
srvd_boolean_t srvd_service_nss_passwd_response_dir_set(srvd_service_response_t *,
                                                        const char *, size_t);
srvd_boolean_t srvd_service_nss_passwd_response_shell_set(srvd_service_response_t *,
                                                          const char *, size_t);
srvd_boolean_t srvd_service_nss_passwd_response_gecos_set(srvd_service_response_t *,
                                                          const char *, size_t);

#endif
