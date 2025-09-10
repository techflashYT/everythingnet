/*
 * EverythingNet - Main App - Global state handling
 * Copyright (C) 2025 Techflash
 */

#include <stddef.h>
#include <evrnet/state.h>

state_t G_State;

void STATE_Init(void) {
	G_State.exitCallbacks.snd  = NULL;
	G_State.exitCallbacks.gfx  = NULL;
	G_State.exitCallbacks.plat = NULL;
	G_State.exitCallbacks.exit = NULL;
}
