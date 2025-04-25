#ifndef _STATE_H
#define _STATE_H

/*
 * Global state
 */
typedef struct {
	/*
	 * Exit callbacks
	 * Will always be called in the following order:
	 * - Sound
	 * - Graphics
	 * - Platform
	 * - Exit
	 *
	 * Functions will not be called if they are NULL.
	 */
	struct __callbackStruct {
		/*
		 * Clean up and shut down anything platform-specific.
		 */
		void (*plat)();

		/*
		 * Clean up and shut down anything relating to the
		 * graphics subsystem currently in use.
		 */
		void (*gfx)();

		/*
		 * Clean up and shut down anything relating to the
		 * audio subsystem currently in use.
		 */
		void (*snd)();

		/*
		 * Everything application specific, platform specific,
		 * graphics specific, and sound specific has been shut
		 * down.  This function is the final thing called by
		 * the app.  It is expected to halt execution of
		 * everythingnet in one way or another.  For example, on
		 * Linux, this with perform the standard exit() call.
		 * On a microcontroller, this might cut the power
		 * (if possible), or spin in a loop forever until the user,
		 * or some other piece of hardware, cuts the power.
		 */
		void (*exit)(int status);
	} exitCallbacks;
} state_t;
extern state_t G_State;

/*
 * Initialize the global state.
 */
extern void STATE_Init();

/*
 * Clean up, shut everything down,
 * call exit callbacks, and exit
 * the application.
 *
 * FIXME: This should probably be in another header
 */
extern void APP_CleanupAndExit(int status);

#endif
