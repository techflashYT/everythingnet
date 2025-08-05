/*
 * EverythingNet - Common headers - Platform specific features - Linux
 * Copyright (C) 2025 Techflash
 */

#ifndef _LINUX_H
#define _LINUX_H

/* don't do anything if not on linux */
#ifdef CONFIG_PLAT_LINUX

extern int LINUX_GatherInfo();
extern int LINUX_GatherCPUInfo();
extern int LINUX_GatherDeviceInfo();

#endif

#endif
