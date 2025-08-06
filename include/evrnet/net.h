/*
 * EverythingNet - Common headers - Networking
 * Copyright (C) 2025 Techflash
 */

#ifndef _NET_H
#define _NET_H

#include <netinet/in.h>
#include <stdint.h>
#include <evrnet/nodeType.h>
#include <evrnet/plat.h>

#define EVRNET_BCAST_PORT 6000
#define EVRNET_MCAST_PORT 6000
#define EVRNET_MCAST_ADDR "239.0.0.1"
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

#ifdef EVRNET_CPU_IS_LE
/* we need to have a function to swap bytes, however,
 * we must beware that some platforms already provide this capability.
 */
#  ifndef __swap32
#    define EVRNET_NEED_SWAP32
#  endif

/* Some platforms have a __swap32, but we can't use it for
 * whatever reason.
 */
#  ifdef CONFIG_PLAT_NEEDS_INTERNAL_SWAP32
#    ifdef __swap32
#      undef __swap32
#    endif
#    define EVRNET_NEED_SWAP32
/* preparation to support swapping on either endianness just in case */
#  elif defined(EVRNET_CPU_IS_BE)
#    ifndef EVRNET_NEED_SWAP32
/* we cant cheat with ntohl on BE */
#      define EVRNET_NEED_SWAP32
#    endif
#  elif defined(EVRNER_CPU_IS_LE)
/* we can cheat with ntohl on LE */
#    ifdef EVRNET_NEED_SWAP32
#      undef EVRNET_NEED_SWAP32
#    endif
#    ifdef __swap32
#      undef __swap32
#    endif
#    define __swap32 ntohl
#  endif

#  ifdef EVRNET_NEED_SWAP32
static inline uint32_t __evrnet_swap32(uint32_t in) {
	return (uint32_t)((in << 24) | ((in << 8) & 0x00FF0000) |
			((in >> 8) & 0x0000FF00) | (in >> 24));
}
#    define __swap32 __evrnet_swap32
/* clean up after ourselves */
#    undef EVRNET_NEED_SWAP32
#  endif

/* we need to have a function to swap bytes, however,
 * we must beware that some platforms already provide this capability.
 */
#  ifndef __swap64
#    define EVRNET_NEED_SWAP64
#  endif

/* Some platforms have a __swap64, but we can't use it for
 * whatever reason.
 */
#  ifdef CONFIG_PLAT_NEEDS_INTERNAL_SWAP64
#    ifdef __swap64
#      undef __swap64
#    endif
#    ifndef EVRNET_NEED_SWAP64
#      define EVRNET_NEED_SWAP64
#    endif
#  endif


#  ifdef EVRNET_NEED_SWAP64
static inline uint64_t __evrnet_swap64(uint64_t in) {
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
#    define __swap64 __evrnet_swap64
#  endif

#  ifndef ntohq
#    define EVRNET_NEED_NTOHQ
#  endif

#  ifndef htonq
#    define EVRNET_NEED_HTONQ
#  endif

/* Some platforms have an ntohq, but we can't use it for
 * whatever reason.
 */
#  ifdef CONFIG_PLAT_NEEDS_INTERNAL_NTOHQ
#    ifdef ntohq
#      undef ntohq
#    endif
#    ifndef EVRNET_NEED_NTOHQ
#      define EVRNET_NEED_NTOHQ
#    endif
#  endif

/* Some platforms have an htonq, but we can't use it for
 * whatever reason.
 */
#  ifdef CONFIG_PLAT_NEEDS_INTERNAL_HTONQ
#    ifdef htonq
#      undef htonq
#    endif
#    ifndef EVRNET_NEED_HTONQ
#      define EVRNET_NEED_HTONQ
#    endif
#  endif


#  ifdef EVRNET_NEED_NTOHQ
#    define ntohq(x) __swap64(x)
#  endif
#  ifdef EVRNET_NEED_HTONQ
#    define htonq(x) __swap64(x)
#  endif

#else
#  ifdef EVRNET_NEED_NTOHQ
#    define ntohq(x) x
#  endif
#  ifdef EVRNET_NEED_HTONQ
#    define htonq(x) x
#  endif
#endif

#endif
