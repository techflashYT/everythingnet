/*
 * EverythingNet - Common headers - Platform specific features - ARM CPU List
 * Copyright (C) 2025 Techflash
 */

#ifndef _ARM_CPUS_LIST_H
#define _ARM_CPUS_LIST_H
#include <evrnet/plat.h> /* for the CPU macros */

/* don't do anything if not on arm */
#if defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)

extern char *armCPUNames[][2];

#endif

#endif
