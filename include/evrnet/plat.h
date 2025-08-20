/*
 * EverythingNet - Common headers - Platform specific features
 * Copyright (C) 2025 Techflash
 */

#ifndef _PLAT_H
#define _PLAT_H
#include <stdint.h>
#include <evrnet/netType.h> /* for network struct */
#include <evrnet/plat_detect.h> /* compat w/ old code that expects plat.h to have these */

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
 * Check for new broadcast / multicast data
 *
 * Returns:
 * < 0: Error
 * 0: No new messages
 * 1: New message available, filled in msg
 */
extern int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg);
extern int PLAT_NetCheckMcastData(evrnet_bcast_msg_t *msg);


/*
 * Do a broadcast / multicast to all known interfaces
 *
 * Returns:
 * < 0: Error
 * 0: Success
 */
extern int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg);
extern int PLAT_NetDoMulticast(evrnet_bcast_msg_t *msg);

/*
 * Start a timer / save the current time
 * Needs to be able to be able to later be used by PLAT_EndTimer
 * to get the difference in microseconds in time passed between
 * this function call and PLAT_EndTimer.
 * How this is ultimately chosen to be implemented is up to the
 * underlying platform - the app glue does not care what this
 * function really does.
 */
extern void PLAT_StartTimer(void);

/*
 * End the timer and return the elapsed time
 * Returns the elapsed time in microseconds between the previous
 * PLAT_StartTimer call and this call.  Any temporary data structures
 * used for the timer may now be destroyed, as this will only be called
 * once, after PLAT_StartTimer.
 */
extern uint32_t PLAT_EndTimer(void);

#endif
