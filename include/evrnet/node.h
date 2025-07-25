#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>
#include <evrnet/plat.h>
#include <evrnet/net.h>
#include <evrnet/nodeType.h>

#define EVRNET_NODELIST_V1 0x00000001

/* Local UUID */
extern uint64_t NODE_LocalUUID[2];

/* Local name */
extern char NODE_LocalName[64];

/* Node list */
extern nodeList_t *NODE_NodeList;

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


#define ENTRY_SIZE(x) ((uint16_t *)x)
#define ENTRY_NUM_IP(x) (((uint8_t *)((uintptr_t)x + 2)))
#define ENTRY_NODE_NAME(x) ((char *)((uintptr_t)x + 3))
#define ENTRY_NODE_IPS(x) \
((uint32_t *)( \
	ALIGN4( \
		(uintptr_t)(ENTRY_NODE_NAME(x)) + strlen(ENTRY_NODE_NAME(x) + 1) \
	) \
))
#define ENTRY_NODE_UUID(x) ((uint64_t *)( \
	ALIGN8( \
		(uintptr_t)(ENTRY_NODE_IPS(x)) + ( \
			*ENTRY_NUM_IP(x) * sizeof(uint32_t) \
		) \
	) \
))
#define ENTRY_NODE_EVRNET_VER(x) ((char *)( \
	((uintptr_t)ENTRY_NODE_UUID(x)) + (sizeof(uint64_t) * 2) \
))

#define ENTRY_NODE_PLATINFO_CAP(x) ((uint32_t *)( \
	ALIGN4( \
		((uintptr_t)ENTRY_NODE_EVRNET_VER(x)) + strlen(ENTRY_NODE_EVRNET_VER(x) + 1) \
	) \
))

#define ENTRY_NODE_PLATINFO_MEMSZ(x) ((int *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_CAP(x) + sizeof(uint32_t) \
))

#define ENTRY_NODE_PLATINFO_NAME(x) ((char *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_MEMSZ(x) + sizeof(int) \
))

#define ENTRY_NODE_PLATINFO_OS(x) ((char *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_NAME(x) + \
	strlen(ENTRY_NODE_PLATINFO_NAME(x)) + 1 \
))

#define ENTRY_NODE_PLATINFO_ARCH(x) ((char *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_OS(x) + \
	strlen(ENTRY_NODE_PLATINFO_OS(x)) + 1 \
))

#define ENTRY_NODE_PLATINFO_CPU(x) ((char *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_ARCH(x) + \
	strlen(ENTRY_NODE_PLATINFO_ARCH(x)) + 1 \
))

#define ENTRY_NODE_PLATINFO_GPU(x) ((char *)( \
	(uintptr_t)ENTRY_NODE_PLATINFO_CPU(x) + \
	strlen(ENTRY_NODE_PLATINFO_CPU(x)) + 1 \
))

#define ENTRY_NEXT(x) ((uint8_t *)( \
	(x + *ENTRY_SIZE(x)) \
))

#define ENTRY_PREV(x) ((uint8_t *)( \
	(x - *ENTRY_SIZE(x)) \
))

#define ENTRY_CALC_SIZE(x) ((uint16_t)( \
	ALIGN8( \
		( \
			((uintptr_t)ENTRY_NODE_PLATINFO_GPU(x) + \
			strlen(ENTRY_NODE_PLATINFO_GPU(x)) + 1) - (uintptr_t)x \
		) \
	) \
))

#endif
