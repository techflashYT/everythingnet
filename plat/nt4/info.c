/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <windows.h>

#include <evrnet/cap.h>
#include <evrnet/plat.h>
#include <evrnet/state.h>

platInfo_t PLAT_Info = {
	NULL, /* name */
	NULL, /* OS (we're going to pull the exact rev) */
	PLAT_STR_NATIVE, /* arch */
	NULL, /* cpu */
	NULL, /* gpu */
	0,    /* memSz */
	/* cap (we're going to fill in some bits
	 * like GPU accel and sound at runtime)
	 */
	CAP_HOST | CAP_DISP | CAP_INPUT | CAP_INPUT_REL | CAP_INPUT_ABS
};

void NT4_GatherDeviceInfo(void) {
	char *tmpStr;
	MEMORYSTATUS memstat;
	DWORD dwVersion, dwMajorVersion, dwMinorVersion, dwBuild, dwNameSz;

	/* get CPU name */
	tmpStr = getenv("PROCESSOR_IDENTIFIER");

	/* save it in PLAT_Info */
	if (tmpStr)
		PLAT_Info.cpu = tmpStr;
	else
		PLAT_Info.cpu = "Unknown";

	/* get memory stats */
	GlobalMemoryStatus(&memstat);

	/* save it in PLAT_Info */
	PLAT_Info.memSz = memstat.dwTotalPhys / 1024;


	/* get Windows version */
	dwVersion = GetVersion();

	/* pick it apart */
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	/* get build number */
	if (dwVersion < 0x80000000)
		dwBuild = (DWORD)(HIWORD(dwVersion));

	/* allocate space for stuff */
	PLAT_Info.os = malloc(64);
	PLAT_Info.name = malloc(64);
	if (!PLAT_Info.os || !PLAT_Info.name) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	/* format it in */
	sprintf(PLAT_Info.os, "Microsoft Windows NT %d.%d (build %d)", dwMajorVersion, dwMinorVersion, dwBuild);

	/* stub out GPU for now */
	PLAT_Info.gpu = "Unknown";

	GetComputerNameA((char *)PLAT_Info.name, &dwNameSz);
}
