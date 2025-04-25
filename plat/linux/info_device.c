#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <evrnet/plat.h>

#if defined(__x86_64__) || defined(__i386__)
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
#endif

int LINUX_GatherDeviceInfo() {
	char scratchBuf[256];
	int fd, bytesRead, curLen;
#if defined(__x86_64__) || defined(__i386__)
	int dmiPathIdx = 0;
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

#if defined(__x86_64__) || defined(__i386__)
	/* grab info from DMI if available */

	/* try to get the vendor */
	fd = open("/sys/devices/virtual/dmi/id/sys_vendor", O_RDONLY);

	/* no vendor to grab, still try to get the model */
	if (fd <= 0)
		goto noDmiVendor;

	/* we have a vendor, read it in */
	bytesRead = read(fd, PLAT_Info.name, 128);

	/* failed */
	if (bytesRead == -1) {
		close(fd);

		/* in case we got partial data */
		memset(PLAT_Info.name, 0, 128);
		goto noDmiVendor;
	}

	if (bytesRead != 0 &&
	   (PLAT_Info.name[bytesRead - 1] == '\r' ||
	    PLAT_Info.name[bytesRead - 1] == '\n'))
		PLAT_Info.name[bytesRead - 1] = 0; /* remove the newline */

	/* null terminate it
	 * if we read nothing, this is harmless, it'll write the index 0
	 * we already check for -1 (fail) above, so this should never blow up
	 */
	PLAT_Info.name[bytesRead] = 0;

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
	if (fd <= 0) {
		goto tryNextPath;
	}

	/* we have a model, read it in */
	bytesRead = read(fd, scratchBuf, (128 - curLen));

	/* failed */
	if (bytesRead == -1) {
		close(fd);
		goto tryNextPath;
	}

	if (bytesRead != 0 &&
	   (scratchBuf[bytesRead - 1] == '\r' ||
	    scratchBuf[bytesRead - 1] == '\n'))
		scratchBuf[bytesRead - 1] = 0; /* remove the newline */

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
	scratchBuf[bytesRead] = 0;

	/* copy it over */
	strncat(PLAT_Info.name, scratchBuf, (128 - curLen));

#elif defined(__powerpc__) || defined(__PPC__)
	/* no-op, it's handled, counterintuitively, in info_cpu.c,
	 * as the model is listed in /proc/cpuinfo...
	 */
#else
#error "Architecture implementation not yet defined"
#endif


	return 0;
}
