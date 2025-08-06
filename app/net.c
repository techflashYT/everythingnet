/*
 * EverythingNet - Main App - Networking core layer
 * Copyright (C) 2025 Techflash
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <evrnet/net.h>
#include <evrnet/plat.h>
#include <evrnet/node.h>
#include <evrnet/state.h>

static evrnet_bcast_msg_t *msg;
static uint32_t counter = 0;
static bool knownBadLocalName = false;

void NET_Init(void) {
	msg = malloc(CONFIG_NET_MAX_PKT_KB * 1024);
	if (!msg) {
		perror("malloc");
		PLAT_FlushOutput();
		APP_CleanupAndExit(1);
	}
	memset(msg, 0, CONFIG_NET_MAX_PKT_KB * 1024);
}

void NET_HandleBcast(void) {
	int ret;

	while (1) {
		/* Check for anything from other nodes */
		ret = PLAT_NetCheckBcastData(msg);
		if (ret != 1) {
#ifdef CONFIG_PLAT_SUPPORTS_MULTICAST
			ret = PLAT_NetCheckMcastData(msg);
			if (ret != 1)
#endif
				break;
		}

		/* Swap the data back to native endianess */
		msg->magic = ntohl(msg->magic);
		msg->crc = ntohl(msg->crc);
		msg->uuid[0] = ntohq(msg->uuid[0]);
		msg->uuid[1] = ntohq(msg->uuid[1]);

		if (msg->uuid[0] == NODE_LocalUUID[0] &&
			msg->uuid[1] == NODE_LocalUUID[1])
			break; /* loopback */

		/* TODO: verify CRC */

		/* Check for new nodes that may have been discovered in this message */
		NODE_CheckForNewNodes(msg);
	}

	if (counter % 15 != 0)
		goto out;

	msg->magic = htonl(EVRNET_BCAST_MAGIC);
	msg->uuid[0] = htonq(NODE_LocalUUID[0]);
	msg->uuid[1] = htonq(NODE_LocalUUID[1]);
	msg->crc = 0;
	/* TODO: Calculate CRC */

	/* convert the node list to BE so we can send it (this will no-op on BE-native platforms) */
	NODE_ListToBE(NODE_NodeList);

	/* copy the now BE-ified node list to our data portion */
	memcpy(&msg->nodeList, NODE_NodeList, ntohl(NODE_NodeList->len));

	/* convert the list back to native endianness so we can use it elsewhere
	 * (this will again no-op on BE-native platforms, since we never swapped
	 *  it in the first place in order to send it).
	 */
	NODE_ListToNative(NODE_NodeList);

	/* our packet is ready, and the node list is back in order.  let's do this thing. */
	PLAT_NetDoBroadcast(msg);

out:
	counter++;

	return;
}
