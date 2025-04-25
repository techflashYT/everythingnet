#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <evrnet/plat.h>

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

int LINUX_GatherCPUInfo() {
	char *cpuInfo, *cpuName, *tmp;
	int ret, fd;

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
#if defined(__x86_64__) || defined(__i386__)
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
#elif defined(__powerpc__) || defined (__PPC__)
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
	strcpy(PLAT_Info.name, "Unknown");

ppcGotModel:
	/* we got something! */
	strcpy(PLAT_Info.name, tmp);


#else
#error "No cpuinfo parsing for this architecture yet, please submit an issue request with your device name, and a copy of /proc/cpuinfo!"
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
	tmp = strstr(cpuName, " with Radeon Graphics");
	if (tmp)
		*tmp = '\0'; /* cut off the string there */

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
