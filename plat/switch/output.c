/*
 * EverythingNet - Switch Platform Specific Features - Output handling
 * Copyright (C) 2025 Techflash
 */

#include <switch.h>
void PLAT_FlushOutput(void) {
	consoleUpdate(NULL);
}
