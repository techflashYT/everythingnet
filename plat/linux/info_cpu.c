/*
 * EverythingNet - Linux Platform Specific Features - Info Gathering - CPU
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <evrnet/plat.h>

#if defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
#include <evrnet/plat/arm-cpus-list.h>
#include <sys/stat.h>
#endif

static int cpuinfoFindLine(const char* cpuinfo, char *out, const char *lineToFind) {
	char *line, *buf, *tmp;
	int size;

	if (!cpuinfo || !lineToFind)
		return -1;

	line = malloc(512);
	if (!line)
		return -1;

	buf = (char *)cpuinfo;
	while (1) {
		/* if we're starting with a \n, then
		 * size=0, and we get caught in an infinite
		 * loop.  sanity check ourselves first.
		 */
		if (buf[0] == '\n')
			buf++; /* *phew* */

		/* find next line */
		tmp = strchr(buf, '\n');
		if (!tmp)
			break;

		size = tmp - buf;

		/* copy it in */
		memcpy(line, buf, size);

		/* null terminate + remove newline */
		memcpy(line, buf, size);
		line[size] = '\0';

		if (line[size - 1] == '\r' || line[size - 1] == '\n')
			line[size - 1] = '\0'; /* nuke the newline */

		if (strncmp(line, lineToFind, strlen(lineToFind)) == 0) {
			char *colon = strchr(line, ':');
			if (!colon) { /* ??? */
				buf += size;
				continue;
			}

			strcpy(out, colon + 2);

			free(line);
			return 0;
		}

		/* next line! */
		buf += size;
	}

	/* well crap, we've got nothing */
	free(line);
	return -1;
}

int LINUX_GatherCPUInfo(void) {
	char *cpuInfo, *cpuName, *tmp;
	int ret, fd;
#if defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
	int dtFd, i, len;
	struct stat statbuf;
#endif

	/* we don't know how big /proc/cpuinfo is, it's
	 * impossible to know ahead of time since it's
	 * constantly changing.  Allocate a worst-case
	 * absurd max size of 1MB here.  We don't need
	 * to keep it around forever anyways, so I consider
	 * this acceptable for a temporary allocation, so
	 * that it's capable of running on the huge CPUs,
	 * like EPYCs, ThreadRippers, Xeons, etc, with
	 * >= 32 cores (some are even approaching 256 now...)
	 *
	 * For reference, my 16-thread Ryzen 5700U has a 25KB
	 * /proc/cpuinfo.  If someone has a 256-thread chip,
	 * that'd be about 400KB.  So 1MB is plenty futureproof.
	 *
	 * Perhaps it'd be nice to make this configurable
	 * so that ultra low end systems don't need to allocate
	 * loads of memory for it when their /proc/cpuinfo is
	 * only few KB at worst.
	 */
	cpuInfo = malloc(1024 * 1024);
	cpuName = malloc(512);
	tmp = malloc(512);
	if (!cpuInfo || !cpuName || !tmp) {
		/* sorry, my low-end friends... */
		perror("malloc");
		return -ENOMEM;
	}

	/* grab the contents */
	fd = open("/proc/cpuinfo", O_RDONLY);
	if (fd < 0) {
		/* weird... */
		ret = errno; /* save errno, perror might clobber it */
		perror("open /proc/cpuinfo failed");
		return ret * -1; /* return negative of failure */
	}

	ret = read(fd, cpuInfo, 1024 * 1024);
	if (ret <= 0) {
		/* also very werid... */
		ret = errno; /* save errno, perror might clobber it */
		perror("read /proc/cpuinfo failed");
		return ret * -1; /* return negative of failure */
	}

	close(fd);

	/* get name of CPU */
#if defined(EVRNET_CPU_X86) || defined(EVRNET_CPU_X86_64)
	ret = cpuinfoFindLine(cpuInfo, tmp, "model name");
	if (ret < 0) {
		/* hmm, no model?  weird */
		PLAT_Info.cpu = "Unknown";
		free(cpuInfo);
		free(tmp);
		return -ENODATA;
	}

	if (ret == 0)
		strcpy(cpuName, tmp);
#elif defined(EVRNET_CPU_PPC32) || defined(EVRNET_CPU_PPC64) || defined(EVRNET_CPU_PPC64LE)
	ret = cpuinfoFindLine(cpuInfo, tmp, "cpu");
	if (ret < 0) {
		/* hmm, no model?  weird.  At least try
		 * to grab the model below though...
		 */
		strcpy(cpuName, "Unknown");
	}

	if (ret == 0)
		strcpy(cpuName, tmp);

	/* but wait!  while we're here, grab the model, since
	 * it's in here as well.
	 *
	 * e.g.
	 * [...]
	 * platform		: PowerMac
	 * model		: PowerBook6,7
	 * machine		: PowerBook6,7
	 * motherboard		: PowerBook6,7 MacRISC3 Power Macintosh 
	 * detected as		: 287 (iBook G4)
	 * [...]
	 *
	 * or
	 * [...]
	 * platform		: wii
	 * model		: nintendo,wii
	 * vendor		: IBM
	 * machine		: Nintendo Wii
	 * [...]
	 *
	 * Try to get the field in this order, ranging from
	 * most to least useful:
	 * - machine
	 * - model
	 * - platform
	 * - motherboard
	 * - detected as
	 */

	ret = cpuinfoFindLine(cpuInfo, tmp, "machine");
	if (ret == 0) goto ppcGotModel;

	ret = cpuinfoFindLine(cpuInfo, tmp, "model");
	if (ret == 0) goto ppcGotModel;

	ret = cpuinfoFindLine(cpuInfo, tmp, "platform");
	if (ret == 0) goto ppcGotModel;

	ret = cpuinfoFindLine(cpuInfo, tmp, "motherboard");
	if (ret == 0) goto ppcGotModel;

	ret = cpuinfoFindLine(cpuInfo, tmp, "detected as");
	if (ret == 0) goto ppcGotModel;

	/* nothing?  we're pretty screwed then, we can't know
	 * what device this is.
	 */
	strcpy(tmp, "Unknown");

ppcGotModel:
	/* we got something! */
	strcpy(PLAT_Info.name, tmp);


#elif defined(EVRNET_CPU_ARM) || defined(EVRNET_CPU_AARCH64)
	/* well crap.
	 * /proc/cpuinfo is insanely useless on aarch64.
	 * All you can really get out is an "ARMv8 Processor rev 1 (v8l)"
	 * or similar.
	 *
	 * To work around this, we try to check the devicetree for a
	 * *real* CPU type (e.g. "arm,cortex-a78") first.
	 * 
	 * If we get nothing back, we can give up and use the useless
	 * /proc/cpuinfo.
	 */


	dtFd = open("/sys/firmware/devicetree/base/cpus/cpu@0/compatible", O_RDONLY);
	if (dtFd < 0)
		goto armTryCpuInfo; /* failed */

	ret = fstat(dtFd, &statbuf);
	if (ret != 0) {
		close(dtFd); /* failed */
		goto armTryCpuInfo;
	}

	ret = read(dtFd, tmp, statbuf.st_size);
	if (ret != statbuf.st_size) {
		close(dtFd); /* failed */
		goto armTryCpuInfo;
	}

	/* we got something, try to convert it to a readable format */
	for (i = 0; armCPUNames[i][0] != NULL; i++) {
		len = strlen(tmp);
		if (strlen(armCPUNames[i][0]) < len) len = strlen(armCPUNames[i][0]);
		/*
		printf("a[i] = %p\n" , armCPUNames[i]);
		printf("a[i][0] = %p\n" , armCPUNames[i][0]);*/

		if (strncmp(tmp, armCPUNames[i][0], len) == 0) {
			strcpy(tmp, armCPUNames[i][1]); /* got a conversion! */
			break;
		}
	}

	/* even if we never hit a conversion, `tmp` is still valid, just not
	 * a pretty name...
	 *
	 * Anyways, we're done here, we got a name, let's clean up and head out.
	 */
	close(dtFd);
	goto armGotModel;
	

armTryCpuInfo:
	/* damn, nothing from the DT, let's give a generic
	 * name from /proc/cpuinfo, it's at least better than
	 * "Unknown"....
	 */
	ret = cpuinfoFindLine(cpuInfo, tmp, "model name");
	if (ret == 0) goto armGotModel;

	/* damn, you don't even have anything in /proc/cpuinfo??
	 * welp, we're out of options, slap it with an "Unknown"...
	 */
	strcpy(tmp, "Unknown");

armGotModel:
	/* we got something! */
	strcpy(cpuName, tmp);
#else
#warning "No cpuinfo parsing for this architecture yet, please submit an issue request with your device name, and a copy of /proc/cpuinfo!"
	strcpy(cpuName, "Unknown (" PLAT_STR_NATIVE ")");
#endif
	/* no matter what the platform, we are garuanteed to reach here with:
	 * - cpuName set to something, either a real name or "Unknown"
	 * - cpuClk potentially being valid (not garuanteed)
	 * - tmp being something (we don't care)
	 */

	/* also while we're here, we *could* try to grab the
	 * `clock` field, but that's dynamic, not the machine's
	 * maximum possible clock speed, so it's very unlikely
	 * to actually be useful.
	 */
	free(tmp);
	free(cpuInfo);

	/* try to clean up the CPU name a bit, some manufacturers
	 * put junk in there that we don't care about.
	 * Reuse the `tmp` variable for this, no use wasting
	 * stack space on another one.  This doesn't cause a memory
	 * leak, as we just free()'d it above.
	 */

	/* remove "with Radeon Graphics" */
	if (strlen(cpuName) > strlen(" with Radeon Graphics")) {
		tmp = strstr(cpuName, " with Radeon Graphics");
		if (tmp)
			*tmp = '\0'; /* cut off the string there */
	}

	/* this is most often Intel, specifying the CPU clock
	 * speed, like this:
	 * - Intel(R) Xeon(R) CPU E5-2680 v4 @ 2.40GHz
	 * - Intel(R) Xeon(R) CPU E3-1240 V2 @ 3.40GHz
	 * - Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz
	 * - Intel(R) Core(TM)2 Duo CPU    E8400 @ 3.00GHz
	 *
	 * We'll do that ourselves later, and get the turbo
	 * clock speed, which is the one that actually matters,
	 * not the base clock speed that Intel shoves into the
	 * name.
	 */
	tmp = strchr(cpuName, '@');
	if (tmp)
		tmp[-1] = '\0'; /* yes this is valid!  it goes back one character and nukes the space before the '@' as well */

	/* TODO: get clock speed?? */

	/* clean up after ourselves and actually apply the final name */
	PLAT_Info.cpu = malloc(strlen(cpuName) + 1);
	strcpy(PLAT_Info.cpu, cpuName);
	free(cpuName);

	return 0;
}
