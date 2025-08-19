/*
 * EverythingNet - Main App - Node entry and node list handling
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <evrnet/nodeType.h>
#include <evrnet/node.h>
#include <evrnet/state.h>
#include <evrnet/net.h>
#include <sys/socket.h>
#include <evrnet/cap.h>
#include <evrnet/netType.h>
#include <evrnet/plat.h>
#include <evrnet/endian.h>

nodeList_t *NODE_NodeList;
char NODE_LocalName[64];
uint64_t NODE_LocalUUID[2];
nodeDiscoverySrc_t NODE_DiscoverySource;
uint8_t *NODE_DiscoverySourceNode;

static char *discoverySrcToStr(nodeDiscoverySrc_t src) {
	switch (src) {
		case NODE_DISCOVERY_SOURCE_BROADCAST:
			return "Broadcast";
		case NODE_DISCOVERY_SOURCE_MULTICAST:
			return "Multicast";
		case NODE_DISCOVERY_SOURCE_OTHER_NODE:
			return "Other Node";
		default:
			return "Unknown";
	}
	return "Unknown";
}

static void NODE_DumpEntry(uint8_t *e) {
	int i;
	char capTemp[256], ip[INET_ADDRSTRLEN];
	puts("==== Node dump ====");
	printf("Node discovery source: %s", discoverySrcToStr(NODE_DiscoverySource));
	if (NODE_DiscoverySource == NODE_DISCOVERY_SOURCE_OTHER_NODE && NODE_DiscoverySourceNode) {
		printf(" [discovered by node: %s]", ENTRY_NODE_NAME(NODE_DiscoverySourceNode));
	}
	puts("");


	printf("size value: %u\n", *ENTRY_SIZE(e));
	printf("num IPs: %u\n", *ENTRY_NUM_IP(e));
	printf("distance from sender: %u (%s)\n", *ENTRY_DISTANCE_FROM_ME(e), *ENTRY_DISTANCE_FROM_ME(e) == 0 ? "Local" : "Remote");
	printf("node name: %s\n", ENTRY_NODE_NAME(e));

	/* doesn't work on NT4 due to missing arpa/inet.h .... */
#if 0
	for (i = 0; i < *ENTRY_NUM_IP(e); i++) {
		inet_ntop(AF_INET, &ENTRY_NODE_IPS(e)[i], ip, INET_ADDRSTRLEN);
		printf("IP address: %s\n", ip);
	}
#endif

	#ifdef EVRNET_CPU_IS_64BIT
	printf("UUID 0: 0x%16lX\n", ENTRY_NODE_UUID(e)[0]);
	printf("UUID 1: 0x%16lX\n", ENTRY_NODE_UUID(e)[1]);
	#elif defined(EVRNET_CPU_IS_32BIT)
	printf("UUID 0: 0x%16llX\n", ENTRY_NODE_UUID(e)[0]);
	printf("UUID 1: 0x%16llX\n", ENTRY_NODE_UUID(e)[1]);
	#endif

	printf("everythingnet version: %s\n", ENTRY_NODE_EVRNET_VER(e));

	CAP_Cap2Str(*ENTRY_NODE_PLATINFO_CAP(e), capTemp);
	printf("cap: 0x%08X, %s\n", *ENTRY_NODE_PLATINFO_CAP(e), capTemp);
	printf("memsz: %dKB\n", *ENTRY_NODE_PLATINFO_MEMSZ(e));
	printf("devname: %s\n", ENTRY_NODE_PLATINFO_NAME(e));
	printf("os: %s\n", ENTRY_NODE_PLATINFO_OS(e));
	printf("arch: %s\n", ENTRY_NODE_PLATINFO_ARCH(e));
	printf("cpu: %s\n", ENTRY_NODE_PLATINFO_CPU(e));
	printf("gpu: %s\n", ENTRY_NODE_PLATINFO_GPU(e));
}

void NODE_Init(void) {
	uint8_t *e;
	/* generous buffer to make our local entry */
	NODE_NodeList = malloc(sizeof(nodeList_t) + 4096);
	if (!NODE_NodeList) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	NODE_NodeList->len = sizeof(nodeList_t);
	NODE_NodeList->version = EVRNET_NODELIST_V1;

	memset((uint8_t *)((uintptr_t)NODE_NodeList + sizeof(nodeList_t)), 0, 4096);

	/* set up local entry */
	e = NODE_NodeList->entries;
	*ENTRY_NUM_IP(e) = 0;
	*ENTRY_DISTANCE_FROM_ME(e) = 0;
	strcpy(ENTRY_NODE_NAME(e), NODE_LocalName);
	ENTRY_NODE_UUID(e)[0] = NODE_LocalUUID[0];
	ENTRY_NODE_UUID(e)[1] = NODE_LocalUUID[1];
	strcpy(ENTRY_NODE_EVRNET_VER(e), "somever");

	/* PLAT_Info */
	*ENTRY_NODE_PLATINFO_CAP(e) = PLAT_Info.cap;
	*ENTRY_NODE_PLATINFO_MEMSZ(e) = PLAT_Info.memSz;
	strcpy(ENTRY_NODE_PLATINFO_NAME(e), PLAT_Info.name);
	strcpy(ENTRY_NODE_PLATINFO_OS(e), PLAT_Info.os);
	strcpy(ENTRY_NODE_PLATINFO_ARCH(e), PLAT_Info.arch);
	strcpy(ENTRY_NODE_PLATINFO_CPU(e), PLAT_Info.cpu);
	strcpy(ENTRY_NODE_PLATINFO_GPU(e), PLAT_Info.gpu);

	*ENTRY_SIZE(e) = ENTRY_CALC_SIZE(e);
	NODE_NodeList->len += *ENTRY_SIZE(e);

	/* now that we've calculated it all out, drop our memory usage */
	NODE_NodeList = realloc(NODE_NodeList, NODE_NodeList->len);
	e = NODE_NodeList->entries;
}

static void NODE_CheckLen(uint32_t len) {
	if (len > (CONFIG_NET_MAX_PKT_KB * 1024) - sizeof(evrnet_bcast_msg_t)) {
		/* Houston, we have a problem.
		 * We don't have enough space the fit our known nodes list
		 * into the buffer.
		 *
		 * No matter what we do, there isn't really any recovery from
		 * this.  The node list can only really grow from here
		 * (we'd need space to handle the disconnection packet to shrink it),
		 * so we're likely to start *failing to recieve packets as well* by
		 * the next iteration.
		 *
		 * Instead of trying to salvage the situation (we *very likely can't*),
		 * bail out and give the user some (hopefully) helpful advice to
		 * prevent us from getting into this jam in the first place.
		 */
		fprintf(stderr,
			"Out of space to fit node list in packet! (overbudget by %d bytes)\r\n"
			"Possible solutions:\r\n"
			"  - Consider connecting less devices into the mesh\r\n"
			"  - Consider reducing the length of device names\r\n"
			"  - Rebuild EverythingNet for every device with a larger CONFIG_NET_MAX_PKT_KB (currently %dKB)\r\n"
			"\r\n"
			"We're pretty screwed, not much left to be done here - bailing out.\r\n",
			(CONFIG_NET_MAX_PKT_KB * 1024) - len,
			CONFIG_NET_MAX_PKT_KB
		);
		APP_CleanupAndExit(1);
	}
}


#ifdef EVRNET_CPU_IS_LE
static void NODE_EntryToLE(uint8_t *e) {
	int i;

	/* convert the size */
	*ENTRY_SIZE(e) = E_BEToHost_16(*ENTRY_SIZE(e));

	/* convert IPs */
	for (i = 0; i < *ENTRY_NUM_IP(e); i++)
		ENTRY_NODE_IPS(e)[i] = E_BEToHost_32(ENTRY_NODE_IPS(e)[i]);

	/* convert UUIDs */
	ENTRY_NODE_UUID(e)[0] = E_BEToHost_64(ENTRY_NODE_UUID(e)[0]);
	ENTRY_NODE_UUID(e)[1] = E_BEToHost_64(ENTRY_NODE_UUID(e)[1]);

	/* convert PLAT_Info */
	*ENTRY_NODE_PLATINFO_CAP(e) = E_BEToHost_32(*ENTRY_NODE_PLATINFO_CAP(e));
	*ENTRY_NODE_PLATINFO_MEMSZ(e) = E_BEToHost_32(*ENTRY_NODE_PLATINFO_MEMSZ(e));
}

static void NODE_EntryToBE(uint8_t *e) {
	int i;

	/* convert IPs */
	for (i = 0; i < *ENTRY_NUM_IP(e); i++)
		ENTRY_NODE_IPS(e)[i] = E_HostToBE_32(ENTRY_NODE_IPS(e)[i]);

	/* convert UUIDs */
	ENTRY_NODE_UUID(e)[0] = E_HostToBE_64(ENTRY_NODE_UUID(e)[0]);
	ENTRY_NODE_UUID(e)[1] = E_HostToBE_64(ENTRY_NODE_UUID(e)[1]);

	/* convert PLAT_Info */
	*ENTRY_NODE_PLATINFO_CAP(e) = E_HostToBE_32(*ENTRY_NODE_PLATINFO_CAP(e));
	*ENTRY_NODE_PLATINFO_MEMSZ(e) = E_HostToBE_32(*ENTRY_NODE_PLATINFO_MEMSZ(e));

	/* convert the size */
	*ENTRY_SIZE(e) = E_HostToBE_16(*ENTRY_SIZE(e));
}

void NODE_ListToBE(nodeList_t *nl) {
	uint8_t *e = nl->entries, **entryPtrs;
	int i = 0, max = 0;

	/* we need to save the pointers because:
	 * - we can't iterate forwards (we clobber the size)
	 * - we can't iterate backwards (we don't know the size of node n-1)
	 */
	entryPtrs = malloc(sizeof(void *) * 512);
	if (!entryPtrs) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	while (((uintptr_t)e - (uintptr_t)nl->entries) < (nl->len - sizeof(nodeList_t))) {
		entryPtrs[i] = e;
		e = ENTRY_NEXT(e);
		i++;
	}
	max = i;

	for (i = 0; i < max; i++)
		NODE_EntryToBE(entryPtrs[i]);

	free(entryPtrs);

	nl->version = E_HostToBE_32(nl->version);
	nl->len = E_HostToBE_32(nl->len);
}

void NODE_ListToNative(nodeList_t *nl) {
	uint8_t *e = nl->entries;
	int i;

	nl->version = E_BEToHost_32(nl->version);
	nl->len = E_BEToHost_32(nl->len);

	/* work forwards, converting every entry to native (LE).
	 * Must happen this way since entries inherently
	 * depend on previous ones in order to traverse
	 */
	while ((uintptr_t)e < ((uintptr_t)nl->entries + (nl->len - sizeof(nodeList_t)))) {
		NODE_EntryToLE(e);
		e = ENTRY_NEXT(e);
	}
}

static void NODE_EntryToNative(uint8_t *e) {
	NODE_EntryToLE(e);
}
#else
#define NODE_EntryToNative(x) (void)0
#endif

static void NODE_MaybeAddEntry(uint8_t *e) {
	bool known = false;
	nodeList_t *nl = NODE_NodeList;
	uint8_t *knownEntry, *lastEntry;
	uint32_t lastEntryOffset;

	knownEntry = nl->entries;

	/* do we know this UUID? */
	while ((uintptr_t)knownEntry <
		((uintptr_t)nl->entries + (nl->len - sizeof(nodeList_t)))) {
		if (ENTRY_NODE_UUID(e)[0] == ENTRY_NODE_UUID(knownEntry)[0] &&
			ENTRY_NODE_UUID(e)[1] == ENTRY_NODE_UUID(knownEntry)[1]) {
			/* we know this node */
			known = true;
			break;
		}

		/* not yet, check next one */
		knownEntry = ENTRY_NEXT(knownEntry);
	}

	/* yes, get back to business */
	if (known) {
		return;
	}

	/* reset knownEntry */
	knownEntry = nl->entries;

	/* get the last entry of the list */
	while (((uintptr_t)knownEntry - (uintptr_t)nl->entries) < (nl->len - sizeof(nodeList_t))) {
		lastEntry = knownEntry;
		knownEntry = ENTRY_NEXT(knownEntry);
	}

	/* save lastEntry offset, since we need to cleanse ourselves
	 * of any old pointers after the below realloc().
	 * So, we can save the offset into the entires table into
	 * which the last entry is, since that'll still be valid
	 * in the new array
	 */
	lastEntryOffset = (uintptr_t)lastEntry - (uintptr_t)nl->entries;

	/* realloc the nodelist to fit our new node */
	nl->len += *ENTRY_SIZE(e);
	NODE_NodeList = realloc(NODE_NodeList, nl->len);

	/* cleanse our pointers */
	nl = NODE_NodeList; /* ensure our shorthand 'nl' points to the new one */
	lastEntry = (uint8_t *)((uintptr_t)nl->entries + lastEntryOffset); /* re-apply offset to new list */

	/* pointer to new (unwritten) entry now in knownEntry */
	knownEntry = ENTRY_NEXT(lastEntry);

	/* copy our new entry over */
	memcpy(knownEntry, e, *ENTRY_SIZE(e));

	printf("Found new node: %s", ENTRY_NODE_NAME(e));
	printf(" [discovery source: %s", discoverySrcToStr(NODE_DiscoverySource));
	if (NODE_DiscoverySource == NODE_DISCOVERY_SOURCE_OTHER_NODE && NODE_DiscoverySourceNode) {
		printf(", discovered by node: %s", ENTRY_NODE_NAME(NODE_DiscoverySourceNode));
	}
	puts("]");
}

void NODE_CheckForNewNodes(evrnet_bcast_msg_t *msg) {
	nodeList_t *nl = &msg->nodeList;
	uint8_t *e = nl->entries;

	NODE_ListToNative(nl);

	while (((uintptr_t)e - (uintptr_t)nl->entries) < (nl->len - sizeof(nodeList_t))) {
		NODE_MaybeAddEntry(e);
		e = ENTRY_NEXT(e);
		NODE_DiscoverySource = NODE_DISCOVERY_SOURCE_OTHER_NODE;
		NODE_DiscoverySourceNode = nl->entries;
	}
}
