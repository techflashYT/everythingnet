/*
 * EverythingNet - Linux Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include "evrnet/node.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

#include <evrnet/cap.h>
#include <evrnet/plat.h>
#include <evrnet/plat/linux.h>

/* all other fields filled out at runtime */
platInfo_t PLAT_Info = {
	.arch = PLAT_STR_NATIVE,
	.cap  = CAP_HOST |
		CAP_DISP |
		CAP_GFX_ACCEL |
		CAP_SND  |
		CAP_INPUT |
		CAP_INPUT_REL |
		CAP_INPUT_ABS
};

int LINUX_GatherInfo() {
	struct utsname utsname;
	int ret;
	FILE *fp;

	ret = LINUX_GatherDeviceInfo();
	if (ret)
		return ret;

	/* get the memory size, divide by 1024 to get KB, as the app expects */
	PLAT_Info.memSz = sysconf(_SC_PHYS_PAGES) * (sysconf(_SC_PAGE_SIZE) / 1024);

	/* Get the kernel version */
	uname(&utsname);

	PLAT_Info.os = malloc(128);
	if (!PLAT_Info.os) {
		perror("malloc");
		return -ENOMEM;
	}

	strcpy(PLAT_Info.os, "Linux ");
	strncat(PLAT_Info.os, utsname.release, 128 - 7); /* 128 - "Linux " */
	strncpy(NODE_LocalName, utsname.nodename, sizeof(NODE_LocalName));

	/* XXX: HACK!  On Android, /etc/machine-id won't exist,
	 * so just generate something random to get it running.
	 * This is against the platform guidelines, but it's
	 * just temporary.
	 */
	#ifdef __ANDROID__
	NODE_LocalUUID[0] = rand();
	NODE_LocalUUID[0] <<= 32;
	NODE_LocalUUID[0] += rand();

	NODE_LocalUUID[1] = rand();
	NODE_LocalUUID[1] <<= 32;
	NODE_LocalUUID[1] += rand();
	#else
	fp = fopen("/etc/machine-id", "r");
	if (!fp) {
		perror("fopen /etc/machine-id");
		return -ENOENT;
	}
	#ifdef EVRNET_CPU_IS_64BIT
	if (fscanf(fp, "%016lx", &NODE_LocalUUID[0]) != 1 ||
		fscanf(fp, "%016lx", &NODE_LocalUUID[1]) != 1) {
	#elif defined(EVRNET_CPU_IS_32BIT)
	if (fscanf(fp, "%016llx", &NODE_LocalUUID[0]) != 1 ||
		fscanf(fp, "%016llx", &NODE_LocalUUID[1]) != 1) {
	#else
	#error "Unknown format specifier for pulling UUID for this CPU"
	#endif
			fclose(fp);
			perror("fscanf");
			return -EINVAL;
	}
	fclose(fp);
	#endif

	ret = LINUX_GatherCPUInfo();
	if (ret)
		return ret;

	return 0;
}
