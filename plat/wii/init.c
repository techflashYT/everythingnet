/*
 * EverythingNet - Wii Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

/*
 * Nintendo Wii libogc platform initialization.
 */
#include <gccore.h>
#include <wiiuse/wpad.h>

#include <evrnet/plat/nintendo-ppc.h>

int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	/* Initialize everything common between the GameCube and Wii */
	NPPC_Init();

	/* This function initialises the attached controllers */
	WPAD_Init();
	
	return 0;
}
