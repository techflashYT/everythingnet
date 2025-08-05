/*
 * EverythingNet - Nintendo GameCube/Wii Common Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 *
 * Modified from devkitPro wii-examples 'template'.
 */

#include <stdlib.h>

#include <gccore.h>

#include <evrnet/state.h>
void *xfb = NULL;
GXRModeObj *rmode = NULL;

void NPPC_Init() {
	/* Initialise the video system */
	VIDEO_Init();

	/* Initialize GameCube pads */
	PAD_Init();

	/* Obtain the preferred video mode from the system 
	 * This will correspond to the settings in the Wii menu for the Wii, 
	 * or (???) for the GameCube
	 */
	rmode = VIDEO_GetPreferredMode(NULL);

	/* Allocate memory for the display in the uncached region */
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	/* Initialise the console, required for printf */
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	/*SYS_STDIO_Report(true);*/

	/* Set up the video registers with the chosen mode */
	VIDEO_Configure(rmode);

	/* Tell the video hardware where our display memory is */
	VIDEO_SetNextFramebuffer(xfb);

	/* Make the display visible */
	VIDEO_SetBlack(false);

	/* Flush the video register changes to the hardware */
	VIDEO_Flush();

	/* Wait for Video setup to complete */
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	/* Init state */
	STATE_Init();

	/* set exit callback to just exit (we don't need to de-init anything) */
	G_State.exitCallbacks.exit = exit;
}
