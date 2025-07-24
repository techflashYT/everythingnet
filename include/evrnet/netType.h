#ifndef _NETTYPE_H
#define _NETTYPE_H

#include <stdint.h>
#include <evrnet/nodeType.h>
typedef struct {
	/* magic number, must be EVRNET_BCAST_MAGIC */
	uint32_t magic;

	/* message length, including header */
	uint32_t len;

	/* header format version */
	uint32_t version;

	/* we don't want the UUID to not start at a 64-bit boundary,
	 * some CPUs don't like that, so add some padding here.
	 */
	uint32_t rsrvd__padding1;

	/* 128-bit per-node UUID */
	uint64_t uuid[2];

	/* CRC32 of header and data */
	uint32_t crc;

	/* pad so that string starts on a 64-bit boundary */
	uint32_t rsrvd__padding2;

	/* node name
	 * TODO: Maybe make dynamically sized?
	 * could be bundled in with the node list I guess, though that's mildly ugly
	 * 64 bytes is enough for basicallly any name, and not *too* much data to spend
	 * per-packet.  Especially is fine if the nodelist is actually optimized,
	 * (not wasting 64B per node per packet, which would be a bigger problem)
	 */
	char myName[64];

	/* Node list */
	nodeList_t nodeList;
} __attribute__((packed)) evrnet_bcast_msg_t;
#endif
