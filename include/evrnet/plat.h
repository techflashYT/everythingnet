/*
 * EverythingNet - Common headers - Platform specific features
 * Copyright (C) 2025 Techflash
 */

#ifndef _PLAT_H
#define _PLAT_H
#include <stdint.h> /* for 32/64-bit check */
#include <evrnet/netType.h> /* for network struct */

/*
 * Platform specific initialization function.
 * Must be provided by every platform, even if just a stub.
 *
 * Takes the cmdline arguments (if applicable, or 0, NULL if not),
 * but is not garuanteed to use them.  It will not modify the argv
 * array.
 *
 * It shall perform basic platform initialization, possibly necessary to
 * bring up things like the console, networking, video rendering, etc.
 *
 * Returns 0 on success, or nonzero on failure.
 */
extern int PLAT_Init(int argc, char *argv[]);


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
#define PLAT_STR_PPC64   "PowerPC (64-bit Big Endian)"
#define PLAT_STR_PPC64LE "PowerPC (64-bit Little Endian)"


/* In order of accuracy:
 * - We have __BYTE_ORDER__ and __ORDER_LITTLER_ENDIAN__, and they are equal?
 * - __LITTLE_ENDIAN__ or __LITTLE_ENDIAN are defined?
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
#else
#  error "Unable to determine endianness for this platform, or endianness is neither big nor little"
#endif

#if defined(__WORDSIZE)
#  if __WORDSIZE == 64
#    define EVRNET_CPU_IS_64BIT
#  elif __WORDSIZE == 32
#    define EVRNET_CPU_IS_32BIT
#  else
#    error "__WORDSIZE is unknown value"
#  endif
#elif defined(INTPTR_MAX) && defined(INT64_MAX)
#  if INTPTR_MAX == INT64_MAX
#    define EVRNET_CPU_IS_64BIT
#  elif defined(INT32_MAX)
#    if INTPTR_MAX == INT32_MAX
#      define EVRNET_CPU_IS_64BIT
#    endif
#  endif
#else
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
#elif defined(__PPC__) && defined(EVRNET_CPU_IS_32BIT)
#  define PLAT_STR_NATIVE PLAT_STR_PPC32
#  define EVRNET_CPU_PPC32
#elif defined(__PPC__) && defined(EVRNET_CPU_IS_64BIT) && defined(EVRNET_CPU_IS_BE)
#  define PLAT_STR_NATIVE PLAT_STR_PPC64
#  define EVRNET_CPU_PPC64
#elif defined(__PPC__) && defined(EVRNET_CPU_IS_64BIT) && defined(EVRNET_CPU_IS_LE)
#  define PLAT_STR_NATIVE PLAT_STR_PPC64LE
#  define EVRNET_CPU_PPC64LE
#else
#  error "Unknown architecture"
#endif

/*
 * Platform specific information.
 */
typedef struct {
	/* Device name */
	char *name;

	/* Current operating system */
	char *os;

	/* CPU Architecture */
	char *arch;

	/* Human readable CPU manufacturer (if known),
	 * name, and clock speed (if known).
	 */
	char *cpu;

	/* Human readable GPU manufacturer and name, if one is
	 * present.  Or at least, info about the current display
	 * if not a proper GPU.
	 */
	char *gpu;

	/* Device RAM size in KB */
	int memSz;

	/* Platform capabilites (bitmask) */
	uint32_t cap;
} platInfo_t;

extern platInfo_t PLAT_Info;

#ifdef CONFIG_PLAT_NEEDS_FLUSH_OUTPUT
extern void PLAT_FlushOutput();
#else
/* no-op */
#define PLAT_FlushOutput() (void)0
#endif


/*
 * Check for new broadcast data
 *
 * Returns:
 * < 0: Error
 * 0: No new messages
 * 1: New message available, filled in msg
 */
extern int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg);


/*
 * Do a broadcast to all known interfaces
 *
 * Returns:
 * < 0: Error
 * 0: Success
 */
extern int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg);

#endif
