#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <evrnet/plat.h>

#if defined(EVRNET_CPU_X86) || defined(EVRNET_CPU_X86_64)
static char *dmiPaths[] = {
	"/sys/devices/virtual/dmi/id/product_version",
	"/sys/devices/virtual/dmi/id/product_name",
	"/sys/devices/virtual/dmi/id/product_family",
	NULL
};

static int isGarbageName(const char *toCheck) {
	if (strcmp(toCheck, "To be filled by OEM") == 0 ||
	    strcmp(toCheck, "To be filled by O.E.M.") == 0 ||
	    strcmp(toCheck, "To be filled by O.E.M") == 0 ||
	    strcmp(toCheck, "System Product Name") == 0 ||
	    strcmp(toCheck, "Default string") == 0 ||
	    strcmp(toCheck, "Default String") == 0 ||
	    strcmp(toCheck, "") == 0 ||
	    strcmp(toCheck, " ") == 0)
		return 1; /* garbage */
	return 0; /* something that's *maybe* useful? */
}
#elif defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
#include <sys/stat.h>
#endif

int LINUX_GatherDeviceInfo() {
	int fd, ret;
#if defined(EVRNET_CPU_X86) || defined(EVRNET_CPU_X86_64)
	char scratchBuf[256];
	int dmiPathIdx = 0, curLen;
#elif defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
	struct stat statbuf;
#endif

	/* try to discover platform info via a few methods, in order:
	 * - /sys/devices/virtual/dmi/id (x86 only)
	 * - /proc/cpuinfo (PowerPC only, and yes,
	 *                  cpuinfo has device model info on PPC,
	 *                  weirdly enough.  Also grab the CPU name
	 *                  while we're there...)
	 * - /proc/device-tree/compatible (ARM only)
	 * - "Generic Linux Device" if we have no idea wtf this hardware is
	 */

	/* allocate some space */
	PLAT_Info.name = malloc(128);

	if (!PLAT_Info.name) {
		perror("malloc");
		return -ENOMEM;
	}

	/* zero it out */
	memset(PLAT_Info.name, 0, 128);

#if defined(EVRNET_CPU_X86) || defined(EVRNET_CPU_X86_64)
	/* grab info from DMI if available */

	/* try to get the vendor */
	fd = open("/sys/devices/virtual/dmi/id/sys_vendor", O_RDONLY);

	/* no vendor to grab, still try to get the model */
	if (fd <= 0)
		goto noDmiVendor;

	/* we have a vendor, read it in */
	ret = read(fd, PLAT_Info.name, 128);
	close(fd);

	/* failed */
	if (ret == -1) {
		/* in case we got partial data */
		memset(PLAT_Info.name, 0, 128);
		goto noDmiVendor;
	}

	if (ret != 0 &&
	   (PLAT_Info.name[ret - 1] == '\r' ||
	    PLAT_Info.name[ret - 1] == '\n'))
		PLAT_Info.name[ret - 1] = 0; /* remove the newline */

	/* null terminate it
	 * if we read nothing, this is harmless, it'll write the index 0
	 * we already check for -1 (fail) above, so this should never blow up
	 */
	PLAT_Info.name[ret] = 0;

	/* add a space if we actually succeeded, so that the below
	 * can to add to it.
	 */
	strcat(PLAT_Info.name, " ");

	/* we jump here from above if we didn't get a vendor id from dmi */
noDmiVendor:
	/* save the current length so we can use it after the read */
	curLen = strlen(PLAT_Info.name);

tryGetModel:

	/* try to read the device model */
	fd = open(dmiPaths[dmiPathIdx], O_RDONLY);

	/* no model to grab from this path, try the next one
	 * additional info about this device.
	 */
	if (fd <= 0)
		goto tryNextPath;

	/* we have a model, read it in */
	ret = read(fd, scratchBuf, (128 - curLen));
	close(fd);

	/* failed */
	if (ret == -1)
		goto tryNextPath;

	if (ret != 0 &&
	   (scratchBuf[ret - 1] == '\r' ||
	    scratchBuf[ret - 1] == '\n'))
		scratchBuf[ret - 1] = 0; /* remove the newline */

	/* sanity check it */
	if (isGarbageName(scratchBuf)) {
		/* hack, not really anywhere better to put this
		 * if it's a function, then that goto won't work..
		 * could make it a macro, but that'd be pretty ugly
		 */
tryNextPath:
		/* try the next path */
		dmiPathIdx++;
		if (dmiPaths[dmiPathIdx] == NULL) {
			/* well crap, every possible path is utter dogcrap
			 * that's not useful... fall back to just saying
			 * that it's unknown.  Keep the vendor if we have
			 * it, at least...
			 */
			strcpy(scratchBuf, "Unknown Model");
		}

		/* more to try, let's go at this again... */
		goto tryGetModel;
	}

	/* null terminate it
	 * if we read nothing, this is harmless, it'll write the index 0
	 * we already check for -1 (fail) above, so this should never blow up
	 */
	scratchBuf[ret] = 0;

	/* copy it over */
	strncat(PLAT_Info.name, scratchBuf, (128 - curLen));

#elif defined(EVRNET_CPU_PPC32) || defined(EVRNET_CPU_PPC64) || defined(EVRNET_CPU_PPC64LE)
	/* no-op, it's handled, counterintuitively, in info_cpu.c,
	 * as the model is listed in /proc/cpuinfo...
	 */
#elif defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
	fd = open("/sys/firmware/devicetree/base/model", O_RDONLY);
	if (fd < 0) {
		strcpy(PLAT_Info.name, "Unknown");
		goto noDtModel;
	}

	ret = fstat(fd, &statbuf);
	if (ret != 0) {
		close(fd);
		strcpy(PLAT_Info.name, "Unknown");
		goto noDtModel;
	}


	ret = read(fd, PLAT_Info.name, statbuf.st_size);
	if (ret != statbuf.st_size) {
		close(fd);
		strcpy(PLAT_Info.name, "Unknown");
		goto noDtModel;
	}

noDtModel:
	close(fd);

#else
#warning "Architecture implementation not yet defined"
	strcpy(PLAT_Info.name, "Unknown");
#endif


	return 0;
}
