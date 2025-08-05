/*
 * EverythingNet - Common headers - Platform specific features - Switch - custom perror()
 * Copyright (C) 2025 Techflash
 */

#ifndef _SWITCH_PERROR_H
#define _SWITCH_PERROR_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

static void SWITCH_perror(const char *str) {
	printf("%s: %s\n", str, strerror(errno));
}

#define perror SWITCH_perror

#endif
