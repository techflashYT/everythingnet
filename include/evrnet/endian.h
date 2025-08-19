/*
 * EverythingNet - Common headers - Endianness handling
 * Copyright (C) 2025 Techflash
 */

#ifndef _EVRNET_ENDIAN_H
#define _EVRNET_ENDIAN_H

#include <evrnet/compat.h>

/* figure out how to swap 16-bit vals */
#ifndef __swap16
#  define EVRNET_NEED_SWAP16
#endif

#ifdef CONFIG_PLAT_NEEDS_INTERNAL_SWAP16
#  ifdef __swap16
#    undef __swap16
#  endif
#  define EVRNET_NEED_SWAP16
#endif

#ifdef EVRNET_NEED_SWAP16
static inline uint16_t __evrnet_swap16(uint16_t in) {
	return (uint16_t)((in << 8) | (in >> 8));
}
#  define __swap16 __evrnet_swap16
/* clean up after ourselves */
#  undef EVRNET_NEED_SWAP16
#endif


/* figure out how to swap 32-bit vals */
#ifndef __swap32
#  define EVRNET_NEED_SWAP32
#endif

#ifdef CONFIG_PLAT_NEEDS_INTERNAL_SWAP32
#  ifdef __swap32
#    undef __swap32
#  endif
#  define EVRNET_NEED_SWAP32
#endif

#ifdef EVRNET_NEED_SWAP32
static inline uint32_t __evrnet_swap32(uint32_t in) {
	return (uint32_t)((in << 24) | ((in << 8) & 0x00FF0000) |
			((in >> 8) & 0x0000FF00) | (in >> 24));
}
#  define __swap32 __evrnet_swap32
/* clean up after ourselves */
#  undef EVRNET_NEED_SWAP32
#endif

/* figure out how to swap 64-bit vals */
#ifndef __swap64
#  define EVRNET_NEED_SWAP64
#endif

#ifdef CONFIG_PLAT_NEEDS_INTERNAL_SWAP64
#  ifdef __swap64
#    undef __swap64
#  endif
#  ifndef EVRNET_NEED_SWAP64
#    define EVRNET_NEED_SWAP64
#  endif
#endif


#ifdef EVRNET_NEED_SWAP64
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
#  define __swap64 __evrnet_swap64
#endif

#ifdef EVRNET_CPU_IS_LE
#  define E_BEToHost_64(x) __swap64(x)
#  define E_HostToBE_64(x) __swap64(x)
#  define E_BEToHost_32(x) __swap32(x)
#  define E_HostToBE_32(x) __swap32(x)
#  define E_BEToHost_16(x) __swap16(x)
#  define E_HostToBE_16(x) __swap16(x)
#else
#  define E_BEToHost_64(x) (x)
#  define E_HostToBE_64(x) (x)
#  define E_BEToHost_32(x) (x)
#  define E_HostToBE_32(x) (x)
#  define E_BEToHost_16(x) (x)
#  define E_HostToBE_16(x) (x)
#endif

#endif
