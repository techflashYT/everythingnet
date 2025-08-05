/*
 * EverythingNet - Common headers - Networking types
 * Copyright (C) 2025 Techflash
 */

#ifndef _NETTYPE_H
#define _NETTYPE_H

#include <stdint.h>
#include <evrnet/nodeType.h>
typedef struct {
	/* magic number, must be EVRNET_BCAST_MAGIC */
	uint32_t magic;

	/* CRC32 of header and data */
	uint32_t crc;

	/* 128-bit per-node UUID */
	uint64_t uuid[2];

	/* Node list */
	nodeList_t nodeList;
} __attribute__((packed)) evrnet_bcast_msg_t;
#endif
