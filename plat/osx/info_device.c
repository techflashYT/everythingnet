/*
 * EverythingNet - Mac OS X Platform Specific Features - Info Gathering - Device Name
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sysctl.h>

#include <evrnet/plat.h>

int OSX_GatherDeviceInfo(void) {
	size_t length = 128;
	PLAT_Info.name = malloc(length);
	if (sysctlbyname("hw.model", PLAT_Info.name, &length, NULL, 0)) {
		perror("sysctlbyname for hw.model failed");
		return -ENOSYS;
	}
	return 0;
}
