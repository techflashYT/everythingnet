/*
 * EverythingNet - Linux Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>

#include <evrnet/state.h>
#include <evrnet/plat/posix.h>
#include <evrnet/plat/linux.h>
#include <evrnet/plat/linux_gfx.h>
int PLAT_Init(int argc, char *argv[]) {
	int ret;
	(void)argc;
	(void)argv;

	STATE_Init();
	G_State.exitCallbacks.exit = exit;

	ret = LINUX_GatherInfo();
	if (ret) {
		fprintf(stderr, "FATAL: LINUX_GatherInfo() returned %d, please check the logs above!\n", ret);
		return ret;
	}
	LINUX_GfxInit();

	/* Nothing important to do on Linux yet, just return success.
	 * When this project is more complete, we'll probably
	 * want to do things like initializing an X11/Wayland
	 * window here.
	 */
	POSIX_NetInit();
	return 0;
}
