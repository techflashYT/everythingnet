/*
 * EverythingNet - DOS Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <dpmi.h>

#include <evrnet/plat/dos.h>
#include <evrnet/plat.h>
#include <evrnet/cap.h>

platInfo_t PLAT_Info = {
	.name  = "IBM PC or Compatible",
	.os    = "MS-DOS or Compatible",
	.cpu   = "Intel 80386 or Compatible",
	.memSz = 640,
	.arch  = PLAT_STR_X86,
	.gpu   = "CGA or better (720x480 text mode)",
	.cap   = CAP_DISP      |
		 CAP_SND       |
		 CAP_INPUT     |
		 CAP_INPUT_ABS |
		 CAP_INPUT_REL
};

int DOS_GatherInfo(void) {
	__dpmi_free_mem_info mem;
	DOS_GatherCPUInfo();

	/* get RAM info */
	if (__dpmi_get_free_memory_information(&mem)) {
		puts("WARN: DOS: Failed to get memory info");
		goto devInf;
	}
	PLAT_Info.memSz = (mem.total_number_of_physical_pages * 4) + 640; /* # of 4KB pages in extended mem, but memSz is in 1 KB */

devInf:
	/*DOS_GatherDeviceInfo();*/

	return 0;
}
