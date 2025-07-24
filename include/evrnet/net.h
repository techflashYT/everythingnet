#ifndef _NET_H
#define _NET_H

#include <netinet/in.h>
#include <stdint.h>
#include <evrnet/nodeType.h>

#define EVRNET_BCAST_PORT 6000
#define EVRNET_BCAST_MAGIC 0x92DCC748
#define EVRNET_BCAST_HDR_V1 0x00000001

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

/* Handles receiving any packets from other nodes
 * across any broadcast interface, and transmitting
 * our own data every once in a while.
 *
 * Should be called every 500ms.
 * Will transmit data once every 15 calls.
 */
extern void NET_HandleBcast(void);

#ifdef __BIG_ENDIAN__
/* if on BE we must provide our own __swap32 for __swap64 to use,
 * since we can't just rely on ntohl like we can on LE (it'd no-op)
 */
static inline uint32_t __swap32(uint32_t in) {
	return (uint32_t)((in << 24) | ((in << 8) & 0x00FF0000) |
			((in >> 8) & 0x0000FF00) | (in >> 24));
}
#else
/* we're on LE, so ntohl will happily byteswap a 32-bit value for
 * us, and probably faster (likely w/ inline asm)
 */
#define __swap32 ntohl
#endif

static inline uint64_t __swap64(uint64_t in) {
	uint32_t hi, lo;

	/* separate into 2x 32-bit values and swap them */
	lo = (uint32_t)(in & 0xFFFFFFFF);
	in >>= 32;

	hi = (uint32_t)(in & 0xFFFFFFFF);
	in = __swap32(lo);
	in <<= 32;
	in |= __swap32(hi);
	return in;
}

#ifdef __BIG_ENDIAN__
#define ntohq(x) x
#define htonq(x) x
#else
#define ntohq(x) __swap64(x)
#define htonq(x) __swap64(x)
#endif

#endif
