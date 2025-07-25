#ifndef _NET_H
#define _NET_H

#include <netinet/in.h>
#include <stdint.h>
#include <evrnet/nodeType.h>
#include <evrnet/plat.h>

#define EVRNET_BCAST_PORT 6000
#define EVRNET_BCAST_MAGIC 0x92DCC748

/* Initialize the networking subsystem */
extern void NET_Init(void);

/* Handles receiving any packets from other nodes
 * across any broadcast interface, and transmitting
 * our own data every once in a while.
 *
 * Should be called every 500ms.
 * Will transmit data once every 15 calls.
 */
extern void NET_HandleBcast(void);

#ifdef EVRNET_CPU_IS_BE
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

#ifdef EVRNET_CPU_IS_BE
#define ntohq(x) x
#define htonq(x) x
#else
#define ntohq(x) __swap64(x)
#define htonq(x) __swap64(x)
#endif

#endif
