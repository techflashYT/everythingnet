/*
 * EverythingNet - Main App - Exit handling
 * Copyright (C) 2025 Techflash
 */
#include <stdio.h>
#include <unistd.h>

#include <evrnet/state.h>

void APP_CleanupAndExit(int status) {
	if (G_State.exitCallbacks.snd)  G_State.exitCallbacks.snd();
	if (G_State.exitCallbacks.gfx)  G_State.exitCallbacks.gfx();
	if (G_State.exitCallbacks.plat) G_State.exitCallbacks.plat();
	if (G_State.exitCallbacks.exit) G_State.exitCallbacks.exit(status);

	/* Give it a sec in case it's
	 * asynchronous and just taking
	 * a bit...
	 */
	sleep(1);

	puts("G_State.exitCallbacks.exit is NULL or failed.  Cannot possibly continue.  Hanging.");
	while (1) {}
}
