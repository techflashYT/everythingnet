/*
 * EverythingNet - DOS Platform Specific Features - Initialization
 * Copyright (C) 2025 Techflash
 */

#include <stdlib.h>
#include <string.h>

#include <evrnet/plat/dos.h>
#include <evrnet/node.h>
#include <evrnet/state.h>

int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	STATE_Init();
	G_State.exitCallbacks.exit = exit;

	strcpy(NODE_LocalName, "DOS PC");

	NET_Init();

	DOS_GatherInfo();
	return 0;
}
