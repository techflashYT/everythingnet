/*
 * EverythingNet - GameCube Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

/*
 * Nintendo GameCube libogc platform initialization.
 */
#include <gccore.h>
#include <evrnet/plat/nintendo-ppc.h>

int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	/* Initialize everything common between the GameCube and Wii */
	NPPC_Init();

	return 0;
}
