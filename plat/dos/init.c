/*
 * EverythingNet - DOS Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

#include <stdlib.h>

#include <evrnet/state.h>

int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	STATE_Init();
	G_State.exitCallbacks.exit = exit;

	/*
	 * TODO: Detect CPU, RAM size, DOS version, etc
	 */
	return 0;
}
