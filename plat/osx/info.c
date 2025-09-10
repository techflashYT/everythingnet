/*
 * EverythingNet - Mac OS X Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 *
 * Memory size detection code derived from fastfetch
 * src/detection/memory/memory_apple.c
 * Copyright (c) 2021-2023 Linus Dierheimer 
 * Copyright (c) 2022-2025 Carter Li 
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/stat.h>

#include <evrnet/node.h>
#include <evrnet/cap.h>
#include <evrnet/plat.h>
#include <evrnet/plat/osx.h>

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

int OSX_GatherInfo(void) {
	struct utsname utsname;
	int ret, size, fd;
	uint64_t ramBytes;
	size_t length = sizeof(ramBytes);
	char *buf, *ptr, *ptrEnd, productName[64], productVersion[64];
	struct stat statbuf;

	ret = OSX_GatherDeviceInfo();
	if (ret)
		return ret;

	/* get the memory size, divide by 1024 to get KB, as the app expects */
	if (sysctl((int[]){ CTL_HW, HW_MEMSIZE }, 2, &ramBytes, &length, NULL, 0)) {
		perror("Failed to get memory size with sysctl");
		return -ENOSYS;
	}
	PLAT_Info.memSz = ramBytes / 1024;

	/* Get the kernel version */
	uname(&utsname);

	PLAT_Info.os = malloc(128);
	if (!PLAT_Info.os) {
		perror("malloc");
		return -ENOMEM;
	}

	/* Open the file to get version info */
	fd = open("/System/Library/CoreServices/SystemVersion.plist", O_RDONLY);
	if (fd < 0) {
		perror("Failed to open /System/Library/CoreServices/SystemVersion.plist");
		return -ENOENT;
	}

	ret = fstat(fd, &statbuf);
	if (ret) {
		close(fd);
		perror("stat");
		return -ENOENT;
	}

	buf = malloc(statbuf.st_size + 1);
	if (!buf) {
		close(fd);
		perror("malloc");
		return -ENOMEM;
	}
	/* to null terminate the str */
	memset(buf, 0, statbuf.st_size + 1);

	/* read it in */
	ret = read(fd, buf, statbuf.st_size);
	if (ret != statbuf.st_size) {
		close(fd);
		free(buf);
		perror("read");
		return -ENOMEM;
	}

	/* we don't need that fd anymore, close it */
	close(fd);

	/* parse out what we need */

	/* Get the product name ("Mac OS X" or "macOS") */
	ptr = strstr(buf, "<key>ProductName</key>");
	/* nothing... probably borked file... */
	if (!ptr) {
		strcpy(productName, "Mac OS X");
		goto noProductName;
	}

	/* OK, we found the key, find the next <string> from there */
	ptr = strstr(ptr, "<string>");
	if (!ptr) {
		strcpy(productName, "Mac OS X");
		goto noProductName;
	}

	/* OK, we found the string, now find the end */
	ptrEnd = strstr(ptr, "</string>");
	if (!ptr) {
		strcpy(productName, "Mac OS X");
		goto noProductName;
	}

	/* how many bytes to copy? */
	ptr += 8; /* "<string>" */
	size = (uintptr_t)ptrEnd - (uintptr_t)ptr;
	memset(productName, 0, sizeof(productName));
	memcpy(productName, ptr, size);

noProductName:
	/* now let's do that whole dance again for ProductVersion */

	ptr = strstr(buf, "<key>ProductVersion</key>");
	if (!ptr) {
		strcpy(productVersion, "[Unknown Version]");
		goto noProductVersion;
	}

	ptr = strstr(ptr, "<string>");
	if (!ptr) {
		strcpy(productName, "[Unknown Version]");
		goto noProductVersion;
	}

	ptrEnd = strstr(ptr, "</string>");
	if (!ptr) {
		strcpy(productName, "[Unknown Version]");
		goto noProductVersion;
	}

	ptr += 8; /* "<string>" */
	size = (uintptr_t)ptrEnd - (uintptr_t)ptr;
	memset(productVersion, 0, sizeof(productVersion));
	memcpy(productVersion, ptr, size);

noProductVersion:

	/* "Mac OS X" or "macOS" */
	strcpy(PLAT_Info.os, productName);
	strncat(PLAT_Info.os, " ", 128 - strlen(PLAT_Info.os));

	/* e.g. "10.5.9" or "15.6.1" */
	strncat(PLAT_Info.os, productVersion, 128 - strlen(PLAT_Info.os));
	strncat(PLAT_Info.os, " w/ Darwin ", 128 - strlen(PLAT_Info.os));

	/* Darwin version */
	strncat(PLAT_Info.os, utsname.release, 128 - strlen(PLAT_Info.os));

	strncpy(NODE_LocalName, utsname.nodename, sizeof(NODE_LocalName));

	ret = OSX_GatherCPUInfo();
	if (ret)
		return ret;

	/* FIXME: GPU info? */
	PLAT_Info.gpu = "[Can't get GPU info on OSX]";

	return 0;
}
