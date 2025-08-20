/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Initialization
 * Copyright (C) 2025 Techflash
 */
#include <stdlib.h>
#include <evrnet/state.h>

int PLAT_Init(void) {
	G_State.exitCallbacks.exit = exit;

	return 0;
}
