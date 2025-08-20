/*
 * EverythingNet - Common headers - Nodes
 * Copyright (C) 2025 Techflash
 */

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>
#include <string.h>
#include <evrnet/plat.h>
#include <evrnet/net.h>
#include <evrnet/nodeType.h>
#include <evrnet/compat.h>

#define EVRNET_NODELIST_V1 0x00000001

/* Local UUID */
extern uint64_t NODE_LocalUUID[2];

/* Local name */
extern char NODE_LocalName[64];

/* Node list */
extern nodeList_t *NODE_NodeList;

/* Node discovery source type */
extern nodeDiscoverySrc_t NODE_DiscoverySource;

/* Node discovery source, pointer to node that told us about this one */
extern uint8_t *NODE_DiscoverySourceNode;

/* Initialize the node subsystem */
extern void NODE_Init(void);

/* Check for any new nodes in a broadcast message */
extern void NODE_CheckForNewNodes(evrnet_bcast_msg_t *msg);

/* Endianness handling functions
 * To convert the list from native endianness to Big Endian,
 * (to be transferred over the network), or from Big Endian
 * back to native endianness
 */
#ifdef EVRNET_CPU_IS_BE
/* These do nothing on BE, since native endianness is already
 * what we want to transfer
 */
#define NODE_ListToNative(x) (void)0
#define NODE_ListToBE(x) (void)0
#else
extern void NODE_ListToNative(nodeList_t *nl);
extern void NODE_ListToBE(nodeList_t *nl);
#endif

#define ALIGN4(x) x + (4 - ((uintptr_t)x % 4)) % 4
#define ALIGN8(x) x + (8 - ((uintptr_t)x % 8)) % 8


static inline uint16_t *ENTRY_SIZE(uint8_t *e) {
	return (uint16_t *)e;
}

static inline uint8_t *ENTRY_NUM_IP(uint8_t *e) {
	return (uint8_t *)((uintptr_t)e + 2);
}

static inline uint8_t *ENTRY_DISTANCE_FROM_ME(uint8_t *e) {
	return (uint8_t *)((uintptr_t)e + 3);
}

static inline char *ENTRY_NODE_NAME(uint8_t *e) {
	return (char *)((uintptr_t)e + 4);
}

static inline uint32_t *ENTRY_NODE_IPS(uint8_t *e) {
	return (uint32_t *)(ALIGN4(
		(uintptr_t)(ENTRY_NODE_NAME(e)) + strlen(ENTRY_NODE_NAME(e)) + 1
	));
}

static inline uint64_t *ENTRY_NODE_UUID(uint8_t *e) {
	return (uint64_t *)(ALIGN8(
		(uintptr_t)(ENTRY_NODE_IPS(e)) + (
			*ENTRY_NUM_IP(e) * sizeof(uint32_t)
		)
	));
}

static inline char *ENTRY_NODE_EVRNET_VER(uint8_t *e) {
	return (char *)(
		((uintptr_t)ENTRY_NODE_UUID(e)) + (sizeof(uint64_t) * 2)
	);
}

static inline uint32_t *ENTRY_NODE_PLATINFO_CAP(uint8_t *e) {
	return (uint32_t *)(ALIGN4(
		((uintptr_t)ENTRY_NODE_EVRNET_VER(e)) + strlen(ENTRY_NODE_EVRNET_VER(e)) + 1
	));
}

static inline int *ENTRY_NODE_PLATINFO_MEMSZ(uint8_t *e) {
	return (int *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_CAP(e) + sizeof(uint32_t)
	);
}

static inline char *ENTRY_NODE_PLATINFO_NAME(uint8_t *e) {
	return (char *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_MEMSZ(e) + sizeof(int)
	);
}

static inline char *ENTRY_NODE_PLATINFO_OS(uint8_t *e) {
	return (char *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_NAME(e) +
		strlen(ENTRY_NODE_PLATINFO_NAME(e)) + 1
	);
}

static inline char *ENTRY_NODE_PLATINFO_ARCH(uint8_t *e) {
	return (char *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_OS(e) +
		strlen(ENTRY_NODE_PLATINFO_OS(e)) + 1
	);
}

static inline char *ENTRY_NODE_PLATINFO_CPU(uint8_t *e) {
	return (char *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_ARCH(e) +
		strlen(ENTRY_NODE_PLATINFO_ARCH(e)) + 1
	);
}

static inline char *ENTRY_NODE_PLATINFO_GPU(uint8_t *e) {
	return (char *)(
		(uintptr_t)ENTRY_NODE_PLATINFO_CPU(e) +
		strlen(ENTRY_NODE_PLATINFO_CPU(e)) + 1
	);
}

#define ENTRY_NEXT(x) ((uint8_t *)( \
	(x + *ENTRY_SIZE(x)) \
))

static inline uint16_t ENTRY_CALC_SIZE(uint8_t *e) {
	return (uint16_t)(ALIGN8(
		((uintptr_t)ENTRY_NODE_PLATINFO_GPU(e) +
		strlen(ENTRY_NODE_PLATINFO_GPU(e)) + 1) - (uintptr_t)e
	));
}

#endif
