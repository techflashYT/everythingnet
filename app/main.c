/*
 * EverythingNet - Main App - Initialization and main loop
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#if _POSIX_TIMERS > 0
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <evrnet/plat.h>
#include <evrnet/cap.h>
#include <evrnet/state.h>
#include <evrnet/net.h>
#include <evrnet/node.h>

int main(int argc, char *argv[]) {
	int ret, memSz, suffixNum, i;
	char* suffix[] = { "KB", "MB", "GB" };
	char capStr[256];
	bool keepRunning = true;
	#if _POSIX_TIMERS > 0
	struct timespec startTime, endTime;
	#else
	struct timeval startTime, endTime;
	#endif

	ret = 0; suffixNum = 0;

	ret = PLAT_Init(argc, argv);
	if (ret != 0) {
		printf("PLAT_Init() failed: %d\r\n", ret);
		PLAT_FlushOutput();
		APP_CleanupAndExit(ret);
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

	NODE_Init();

	/* main loop */
	while (keepRunning) {
		long usecToSleep = 500 * 1000; /* 500ms loop time */

		/* get start time (to later calculate time spent doing work) */
		#if _POSIX_TIMERS > 0
		clock_gettime(CLOCK_MONOTONIC, &startTime);
		#else
		gettimeofday(&startTime, NULL);
		#endif

		/* actually do the networking */
		NET_HandleBcast();

		/* show anything we got */
		PLAT_FlushOutput();

		/* TODO: some way to make keepRunning go false on request */

		/* get end time, to calculate time spent doing work
		 * and subtract it from time to sleep until next iteration
		 */
		#if _POSIX_TIMERS > 0
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		usecToSleep -= ((endTime.tv_sec - startTime.tv_sec) * 1000000L) +
						((endTime.tv_nsec - startTime.tv_nsec) / 1000L);
		#else
		gettimeofday(&endTime, NULL);
		usecToSleep -= ((endTime.tv_sec - startTime.tv_sec) * 1000000L) +
						(endTime.tv_usec - startTime.tv_usec);
		#endif

		/* well damn, we must be running on a 4MHz
		 * microcontroller or something, how the
		 * hell did that take so long?
		 * Either way, we're running behind, so don't
		 * sleep at all.
		 *
		 * TODO: nanosleep() is more precise, but harder
		 * to use, and no trivial way to detect if it exists
		 */
		if (usecToSleep > 0)
			usleep(usecToSleep);
	}

	APP_CleanupAndExit(0);
	return 1; /* should never be reached */
}
