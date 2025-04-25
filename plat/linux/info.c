#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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

	ret = LINUX_GatherDeviceInfo();
	if (ret)
		return ret;

	/* get the memory size, divide by 1024 to get KB, as the app expects */
	PLAT_Info.memSz = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / 1024;

	/* Get the kernel version */
	uname(&utsname);

	PLAT_Info.os = malloc(128);
	if (!PLAT_Info.os) {
		perror("malloc");
		return -ENOMEM;
	}

	strcpy(PLAT_Info.os, "Linux ");
	strncat(PLAT_Info.os, utsname.release, 128 - 7); /* 128 - "Linux " */

	ret = LINUX_GatherCPUInfo();
	if (ret)
		return ret;

	return 0;
}
