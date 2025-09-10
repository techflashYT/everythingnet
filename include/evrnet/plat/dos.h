/*
 * EverythingNet - Common headers - Platform specific features - DOS
 * Copyright (C) 2025 Techflash
 */

#ifndef _DOS_H
#define _DOS_H

/* don't do anything if not on DOS */
#ifdef CONFIG_PLAT_DOS

extern int DOS_GatherInfo();
extern void DOS_GatherCPUInfo();

#endif

#endif
