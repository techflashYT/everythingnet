/*
 * EverythingNet - Common Platform-specific features - POSIX-compatible timers
 * Copyright (C) 2025 Techflash
 */

#include <unistd.h>
#include <stdint.h>

#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK) && _POSIX_MONOTONIC_CLOCK > 1
#  define EVRNET_TIMESPEC
#  include <time.h>
struct timespec startTime;
#else
#  define EVRNET_TIMEVAL
#  include <sys/time.h>
struct timeval startTime;
#endif

void PLAT_StartTimer(void) {
	/* get start time (to later calculate time spent doing work) */
	#ifdef EVRNET_TIMESPEC
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	#else
	gettimeofday(&startTime, NULL);
	#endif
}

uint32_t PLAT_EndTimer(void) {
	#ifdef EVRNET_TIMESPEC
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
