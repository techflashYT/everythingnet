/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Initialization
 * Copyright (C) 2025 Techflash
 */
#include <stdlib.h>
#include <evrnet/state.h>
#include <evrnet/plat/nt4.h>
#include <evrnet/net.h>

int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	G_State.exitCallbacks.exit = exit;

	NT4_GatherDeviceInfo();

	NET_Init();
	return 0;
}
