#ifndef _PLAT_H
#define _PLAT_H
#include <stdint.h> /* for 32/64-bit check */

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

#if defined(__i386__) || defined(__x86__) || defined(_M_IX86) || defined(__386__) || defined(__X86__) || defined(__DOS__)
#define PLAT_STR_NATIVE PLAT_STR_X86
#elif defined(__x86_64__)
#define PLAT_STR_NATIVE PLAT_STR_X86_64
#elif defined(__ARM__) || defined (_ARM_) || defined(__arm__) || defined(__arm)
#define PLAT_STR_NATIVE PLAT_STR_ARMV7
#elif defined(__AARCH64__) || defined(__aarch64__)
#define PLAT_STR_NATIVE PLAT_STR_ARMV80
#elif defined(__PPC__) && (INTPTR_MAX == INT32_MAX)
#define PLAT_STR_NATIVE PLAT_STR_PPC32
#elif defined(__PPC__) && (INTPTR_MAX == INT64_MAX) && defined(__BIG_ENDIAN__)
#define PLAT_STR_NATIVE PLAT_STR_PPC64
#elif defined(__PPC__) && (INTPTR_MAX == INT64_MAX) && !defined(__BIG_ENDIAN__) /* __LITTLE_ENDIAN__ may not specifically be defined */
#define PLAT_STR_NATIVE PLAT_STR_PPC64LE
#else
#error "Unknown architecture"
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
	unsigned int cap;
} platInfo_t;

extern platInfo_t PLAT_Info;

#ifdef CONFIG_PLAT_NEEDS_FLUSH_OUTPUT
extern void PLAT_FlushOutput();
#else
/* no-op */
#define PLAT_FlushOutput() (void)0
#endif

#endif
