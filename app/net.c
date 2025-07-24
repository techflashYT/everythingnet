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
		APP_CleanupAndExit(1);
	}
	memset(msg, 0, CONFIG_NET_MAX_PKT_KB * 1024);
}

void NET_HandleBcast(void) {
	int ret;

	/* Check for anything from other nodes */
	ret = PLAT_NetCheckBcastData(msg);
	if (ret != 1)
		goto tx;

	/* Swap the data back to native endianess */
	msg->magic = ntohl(msg->magic);
	msg->len = ntohl(msg->len);
	msg->version = ntohl(msg->version);
	if (msg->version != EVRNET_BCAST_HDR_V1) {
		fprintf(stderr, "Got message with bogus header version %d, ignoring...\r\n", msg->version);
		goto tx;
	}

	msg->crc = ntohl(msg->crc);
	msg->uuid[0] = ntohq(msg->uuid[0]);
	msg->uuid[1] = ntohq(msg->uuid[1]);

	/* TODO: verify CRC */

	/* Check for new nodes that may have been discovered in this message */
	NODE_CheckForNewNodes(msg);

tx:
	if (counter % 15 != 0)
		goto out;

	msg->magic = htonl(EVRNET_BCAST_MAGIC);

	/* we intentionally don't byteswap the length yet to reuse the value for the below check */
	msg->len = sizeof(evrnet_bcast_msg_t) + NODE_NodeList->len;
	if (msg->len > CONFIG_NET_MAX_PKT_KB * 1024) {
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
			(CONFIG_NET_MAX_PKT_KB * 1024) - msg->len,
			CONFIG_NET_MAX_PKT_KB
		);
		APP_CleanupAndExit(1);
	}

	msg->version = htonl(EVRNET_BCAST_HDR_V1);
	msg->rsrvd__padding1 = 0;
	msg->rsrvd__padding2 = 0;
	msg->uuid[0] = htonq(NODE_LocalUUID[0]);
	msg->uuid[1] = htonq(NODE_LocalUUID[1]);
	strncpy(msg->myName, NODE_LocalName, sizeof(msg->myName));
	msg->crc = 0;
	/* TODO: Calculate CRC */

	/* convert the node list to BE so we can send it (this will no-op on BE-native platforms) */
	NODE_ListToBE();

	/* copy the now BE-ified node list to our data portion */
	memcpy(&msg->nodeList, NODE_NodeList, NODE_NodeList->len);

	/* convert the list back to native endianness so we can use it elsewhere
	 * (this will again no-op on BE-native platforms, since we never swapped
	 *  it in the first place in order to send it).
	 */
	NODE_ListToNative();

	/* now that we've used msg->len, convert it to BE to actually be sent over the wire */
	msg->len = htonl(msg->len);

	/* our packet is ready, and the node list is back in order.  let's do this thing. */
	PLAT_NetDoBroadcast(msg);

out:
	counter++;

	return;
}
