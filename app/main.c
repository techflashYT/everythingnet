/*
 * EverythingNet - Main App Startup
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <unistd.h>
#include <evrnet/plat.h>
#include <evrnet/cap.h>
#include <evrnet/state.h>

int main(int argc, char *argv[]) {
	int ret, memSz, suffixNum, i;
	char* suffix[] = { "KB", "MB", "GB" };
	char capStr[256];
	ret = 0; suffixNum = 0;

	ret = PLAT_Init(argc, argv);
	if (ret != 0) {
		printf("PLAT_Init() failed: %d\r\n", ret);
		PLAT_FlushOutput();
		APP_CleanupAndExit(0);
	}

	puts("EverythingNet starting up on...");
	printf("Machine: %s\r\n", PLAT_Info.name);
	printf("OS: %s\r\n", PLAT_Info.os);
	printf("CPU: %s\r\n", PLAT_Info.cpu);
	printf("CPU Architecture: %s\r\n", PLAT_Info.arch);

	memSz = PLAT_Info.memSz;
	for (i = 0; i != 2; i++) {
		if (memSz >= 10240) {
			memSz /= 1024;
			suffixNum++;
		}
	}
	printf("Memory Size: %d%s\r\n", memSz, suffix[suffixNum]);
	printf("GPU: %s\r\n", PLAT_Info.gpu);
	CAP_Cap2Str(PLAT_Info.cap, capStr);
	printf("Capabilities: %s\r\n", capStr);
	PLAT_FlushOutput();

	sleep(5);

	APP_CleanupAndExit(0);
	return 1; /* should never be reached */
}
