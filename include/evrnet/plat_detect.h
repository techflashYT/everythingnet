/*
 * EverythingNet - Common headers - Platform detection
 * Copyright (C) 2025 Techflash
 */

#ifndef _PLAT_DETECT_H
#define _PLAT_DETECT_H
#include <stdint.h>

/*
 * CPU Architecture Names
 */
#define PLAT_STR_X86     "x86 / i386"
#define PLAT_STR_X86_64  "x86_64 / AMD64"
#define PLAT_STR_ARMV4   "ARMv4 / ARM7"
#define PLAT_STR_ARMV5   "ARMv5 / ARM9"
#define PLAT_STR_ARMV6   "ARMv6 / ARM11"
#define PLAT_STR_ARMV7   "ARMv7"
#define PLAT_STR_ARMV80  "ARMv8"
#define PLAT_STR_ARMV81  "ARMv8.1"
#define PLAT_STR_ARMV82  "ARMv8.2"
#define PLAT_STR_ARMV9   "ARMv9"
#define PLAT_STR_PPC32   "PowerPC (32-bit Big Endian)"
#define PLAT_STR_PPC32LE "PowerPC (32-bit Little Endian)"
#define PLAT_STR_PPC64   "PowerPC (64-bit Big Endian)"
#define PLAT_STR_PPC64LE "PowerPC (64-bit Little Endian)"


/* In order of accuracy:
 * - We have __BYTE_ORDER__ and __ORDER_LITTLER_ENDIAN__, and they are equal?
 * - __LITTLE_ENDIAN__ or __LITTLE_ENDIAN are defined?
 * - Are we targetting Microsoft Windows?  (Even old NT RISC ports _always_ ran in LE)
 *
 * And the reverse of the above for BE:
 * - We have __BYTE_ORDER__ and __ORDER_BIG_ENDIAN__, and they are equal?
 * - __BIG_ENDIAN__ or __BIG_ENDIAN are defined?
 */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define EVRNET_CPU_IS_LE 1
#  elif defined(__ORDER_BIG_ENDIAN__)
#    if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      define EVRNET_CPU_IS_BE 1
#    endif
#  endif
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
#  define EVRNET_CPU_IS_BE 1
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
#  define EVRNET_CPU_IS_LE 1
#elif defined(_MSC_VER)
/* NT is (almost) always LE */
#  define EVRNET_CPU_IS_LE 1
#endif

#if !defined(EVRNET_CPU_IS_LE) && !defined(EVRNET_CPU_IS_BE)
#  error "Unable to determine endianness for this platform, or endianness is neither big nor little"
#endif


/*
 * Try to detect CPU word size
 */
#if defined(__WORDSIZE)
#  if __WORDSIZE == 64
#    define EVRNET_CPU_IS_64BIT
#  elif __WORDSIZE == 32
#    define EVRNET_CPU_IS_32BIT
#  else
#    error "__WORDSIZE is unknown value"
#  endif
#elif defined(INTPTR_MAX)
#  if defined(INT64_MAX)
#    if INTPTR_MAX == INT64_MAX
#      define EVRNET_CPU_IS_64BIT
#    endif
#  elif defined(INT32_MAX)
#    if INTPTR_MAX == INT32_MAX
#      define EVRNET_CPU_IS_64BIT
#    endif
#  endif
#elif defined(_MSC_VER)
#  if defined(_WIN64)
#    define EVRNET_CPU_IS_64BIT
#  else
/* FIXME: Should probably try to filter out trying to build w/ Win16... */
#    define EVRNET_CPU_IS_32BIT
#  endif
#endif

#if !defined(EVRNET_CPU_IS_32BIT) && !defined(EVRNET_CPU_IS_64BIT)
#  error "Don't know how to detect CPU word size for this platform"
#endif


#if defined(__i386__) || defined(__x86__) || defined(_M_IX86) || defined(__386__) || defined(__X86__) || defined(__DOS__)
#  define PLAT_STR_NATIVE PLAT_STR_X86
#  define EVRNET_CPU_X86
#elif defined(__x86_64__)
#  define PLAT_STR_NATIVE PLAT_STR_X86_64
#  define EVRNET_CPU_X86_64
#elif defined(__ARM__) || defined (_ARM_) || defined(__arm__) || defined(__arm)
#  define PLAT_STR_NATIVE PLAT_STR_ARMV7 /* FIXME: More granular? */
#  define EVRNET_CPU_ARM
#elif defined(__AARCH64__) || defined(__aarch64__)
#  define PLAT_STR_NATIVE PLAT_STR_ARMV80 /* FIXME: More granular? */
#  define EVRNET_CPU_AARCH64
#elif defined(__POWERPC__) || defined(__PPC__) || defined(_M_PPC) || defined(__powerpc__) || defined(_ARCH_PPC)
#  if defined(EVRNET_CPU_IS_32BIT)
#    if defined(EVRNET_CPU_IS_BE)
#      define PLAT_STR_NATIVE PLAT_STR_PPC32
#      define EVRNET_CPU_PPC32
#    elif defined(EVRNET_CPU_IS_LE)
#      define PLAT_STR_NATIVE PLAT_STR_PPC32LE
#      define EVRNET_CPU_PPC32LE
#    endif
#  elif defined(EVRNET_CPU_IS_64BIT)
#    if defined(EVRNET_CPU_IS_BE)
#      define PLAT_STR_NATIVE PLAT_STR_PPC64
#      define EVRNET_CPU_PPC64
#    elif defined(EVRNET_CPU_IS_LE)
#      define PLAT_STR_NATIVE PLAT_STR_PPC64LE
#      define EVRNET_CPU_PPC64LE
#    endif
#  endif
#else
#  error "Unknown architecture"
#endif


#endif
