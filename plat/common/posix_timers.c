/*
 * EverythingNet - Common Platform-specific features - POSIX-compatible timers
 * Copyright (C) 2025 Techflash
 */

#include <unistd.h>
#include <stdint.h>

#if _POSIX_TIMERS > 0
#include <time.h>
struct timespec startTime;
#else
#include <sys/time.h>
struct timeval startTime;
#endif

void PLAT_StartTimer(void) {
	/* get start time (to later calculate time spent doing work) */
	#if _POSIX_TIMERS > 0
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	#else
	gettimeofday(&startTime, NULL);
	#endif
}

uint32_t PLAT_EndTimer(void) {
	#if _POSIX_TIMERS > 0
	struct timespec endTime;

	clock_gettime(CLOCK_MONOTONIC, &endTime);
	return ((endTime.tv_sec - startTime.tv_sec) * 1000000L) +
		((endTime.tv_nsec - startTime.tv_nsec) / 1000L);
	#else
	struct timeval endTime;

	gettimeofday(&endTime, NULL);
	return ((endTime.tv_sec - startTime.tv_sec) * 1000000L) +
		(endTime.tv_usec - startTime.tv_usec);
	#endif
}

void PLAT_USleep(uint32_t usec) {
	usleep(usec);
}
