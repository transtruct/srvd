/* protocol.h: Basic protocol definitions.
 *
 * This file is part of the nss-daemon NSS backend.
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

#if 0
/* Mail aliases. */
#define NSSD_PROTOCOL_ALIASES_NAME ((nssd_protocol_type_t)11)
#define NSSD_PROTOCOL_ALIASES_ENTITIES ((nssd_protocol_type_t)12)

/* Ethernet numbers. */
#define NSSD_PROTOCOL_ETHERS_TON ((nssd_protocol_type_t)21)
#define NSSD_PROTOCOL_ETHERS_TOHOST ((nssd_protocol_type_t)22)
#define NSSD_PROTOCOL_ETHERS_ENTITIES ((nssd_protocol_type_t)23)

/* User groups. */
#define NSSD_PROTOCOL_GROUPS_NAME ((nssd_protocol_type_t)31)
#define NSSD_PROTOCOL_GROUPS_GID ((nssd_protocol_type_t)32)
#define NSSD_PROTOCOL_GROUPS_MEMBERSHIP ((nssd_protocol_type_t)33)
#define NSSD_PROTOCOL_GROUPS_ENTITIES ((nssd_protocol_type_t)34)

/* Hostnames. */
#define NSSD_PROTOCOL_HOSTS_NAME ((nssd_protocol_type_t)41)
#define NSSD_PROTOCOL_HOSTS_NAME2 ((nssd_protocol_type_t)42)
#define NSSD_PROTOCOL_HOSTS_ADDR ((nssd_protocol_type_t)43)
#define NSSD_PROTOCOL_HOSTS_ENTITIES ((nssd_protocol_type_t)44)

/* Host and user mappings. */
#define NSSD_PROTOCOL_NETGROUPS_ENTITIES ((nssd_protocol_type_t)51)

/* Network names. */
#define NSSD_PROTOCOL_NETWORKS_NAME ((nssd_protocol_type_t)61)
#define NSSD_PROTOCOL_NETWORKS_ADDR ((nssd_protocol_type_t)62)
#define NSSD_PROTOCOL_NETWORKS_ENTITIES ((nssd_protocol_type_t)63)

/* User information. */
#define NSSD_PROTOCOL_PASSWD_NAME ((nssd_protocol_type_t)71)
#define NSSD_PROTOCOL_PASSWD_UID ((nssd_protocol_type_t)72)
#define NSSD_PROTOCOL_PASSWD_ENTITIES ((nssd_protocol_type_t)73)

/* Network protocols. */
#define NSSD_PROTOCOL_PROTOCOLS_NAME ((nssd_protocol_type_t)81)
#define NSSD_PROTOCOL_PROTOCOLS_NUMBER ((nssd_protocol_type_t)82)
#define NSSD_PROTOCOL_PROTOCOLS_ENTITIES ((nssd_protocol_type_t)83)

/* RPC destinations. */
#define NSSD_PROTOCOL_RPC_NAME ((nssd_protocol_type_t)91)
#define NSSD_PROTOCOL_RPC_NUMBER ((nssd_protocol_type_t)92)
#define NSSD_PROTOCOL_RPC_ENTITIES ((nssd_protocol_type_t)93)

/* Network services. */
#define NSSD_PROTOCOL_SERVICES_NAME ((nssd_protocol_type_t)101)
#define NSSD_PROTOCOL_SERVICES_PORT ((nssd_protocol_type_t)102)
#define NSSD_PROTOCOL_SERVICES_ENTITIES ((nssd_protocol_type_t)103)

/* Additional user information. */
#define NSSD_PROTOCOL_SHADOW_NAME ((nssd_protocol_type_t)111)
#define NSSD_PROTOCOL_SHADOW_ENTITIES ((nssd_protocol_type_t)112)
#endif

/* Reserved for internal use. */
#define NSSD_PROTOCOL_RESP_STATUS ((nssd_protocol_type_t)65535)

#endif
