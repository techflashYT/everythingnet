/*
 * EverythingNet - Common headers - Platform specific features - Nintendo GameCube/Wii Common
 * Copyright (C) 2025 Techflash
 */

#ifndef _NINTENDO_PPC_H
#define _NINTENDO_PPC_H

// don't do anything if not on gcn/wii
#ifdef CONFIG_NINTENDO_PPC_COMMON
#include <gccore.h>

extern void NPPC_Init();

extern void *xfb;
extern GXRModeObj *rmode;

#endif

#endif
