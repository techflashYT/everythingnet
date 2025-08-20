/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Timing
 * Copyright (C) 2025 Techflash
 */

#include <stdint.h>
#include <windows.h>

static LARGE_INTEGER timer_start;
static LARGE_INTEGER freq;

void PLAT_StartTimer(void) {
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&timer_start);
}

uint32_t PLAT_EndTimer(void) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (uint32_t)((end.QuadPart - timer_start.QuadPart) * 1000000 / freq.QuadPart);
}

void PLAT_USleep(uint32_t usec) {
    DWORD msec = (usec + 999) / 1000;
    if (msec == 0) msec = 1;
    Sleep(msec);
}
