/*
 * EverythingNet - Mac OS X Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>

#include <evrnet/state.h>
#include <evrnet/plat/osx.h>
int PLAT_Init(int argc, char *argv[]) {
	int ret;
	(void)argc;
	(void)argv;

	STATE_Init();
	G_State.exitCallbacks.exit = exit;

	ret = OSX_GatherInfo();
	if (ret) {
		fprintf(stderr, "FATAL: OSX_GatherInfo() returned %d, please check the logs above!\n", ret);
		return ret;
	}

	OSX_NetInit();
	return 0;
}
