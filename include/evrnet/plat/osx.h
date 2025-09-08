/*
 * EverythingNet - Common headers - Platform specific features - Mac OS X
 * Copyright (C) 2025 Techflash
 */

#ifndef _OSX_H
#define _OSX_H

/* don't do anything if not on OSX */
#ifdef CONFIG_PLAT_OSX

extern int OSX_GatherInfo();
extern int OSX_GatherCPUInfo();
extern int OSX_GatherDeviceInfo();

#endif

#endif
