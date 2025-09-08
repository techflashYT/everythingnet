/*
 * EverythingNet - Mac OS X Platform Specific Features - Info Gathering - CPU
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <mach/machine.h>

#include <evrnet/plat.h>

int OSX_GatherCPUInfo() {
#if defined(EVRNET_CPU_PPC32) || defined(EVRNET_CPU_PPC64)
	/* TODO: How to detect 7447a, 970FX, 745/755, etc? */
	uint32_t cpuFamily, cpuType, cpuSubType;
	size_t length = sizeof(cpuType);
	if (sysctlbyname("hw.cputype", &cpuType, &length, NULL, 0)) {
		perror("sysctlbyname for hw.cputype failed");
		return -ENOSYS;
	}
	if (sysctlbyname("hw.cpusubtype", &cpuSubType, &length, NULL, 0)) {
		perror("sysctlbyname for hw.cpusubtype failed");
		return -ENOSYS;
	}
	if (sysctlbyname("hw.cpufamily", &cpuFamily, &length, NULL, 0)) {
		perror("sysctlbyname for hw.cpufamily failed");
		return -ENOSYS;
	}

	if (cpuType != CPU_TYPE_POWERPC && cpuType != CPU_TYPE_POWERPC64) {
		printf("OSX: Unkown CPU Type for PPC: 0x%08X\n", cpuType);
		PLAT_Info.cpu = "Unknown (PPC?) CPU";
		return -ENOENT;
	}

	switch (cpuFamily) {
		case CPUFAMILY_POWERPC_G3: {
			PLAT_Info.cpu = "PowerPC 750 \"G3\"";
			break;
		}
		case CPUFAMILY_POWERPC_G4: {
			PLAT_Info.cpu = "PowerPC 7400 \"G4\"";
			break;
		}
		case CPUFAMILY_POWERPC_G5: {
			PLAT_Info.cpu = "PowerPC 970 \"G5\"";
			break;
		}
		default: {
			printf("OSX: Unknown CPU Family for PPC: 0x%08X\n", cpuFamily);
			PLAT_Info.cpu = "Unknown PowerPC CPU Family";
			break;
		}
	}

	/* Overrides for cpuSubType */
	switch (cpuSubType) {
		case CPU_SUBTYPE_POWERPC_750: /* we already set these above */
		case CPU_SUBTYPE_POWERPC_7400:
		case CPU_SUBTYPE_POWERPC_970:
			break;
		case CPU_SUBTYPE_POWERPC_601: {
			PLAT_Info.cpu = "PowerPC 601";
			break;
		}
		case CPU_SUBTYPE_POWERPC_602: {
			PLAT_Info.cpu = "PowerPC 602";
			break;
		}
		case CPU_SUBTYPE_POWERPC_603: {
			PLAT_Info.cpu = "PowerPC 603";
			break;
		}
		case CPU_SUBTYPE_POWERPC_603e: {
			PLAT_Info.cpu = "PowerPC 603e";
			break;
		}
		case CPU_SUBTYPE_POWERPC_603ev: {
			PLAT_Info.cpu = "PowerPC 603ev";
			break;
		}
		case CPU_SUBTYPE_POWERPC_604: {
			PLAT_Info.cpu = "PowerPC 604";
			break;
		}
		case CPU_SUBTYPE_POWERPC_604e: {
			PLAT_Info.cpu = "PowerPC 604e";
			break;
		}
		case CPU_SUBTYPE_POWERPC_620: {
			PLAT_Info.cpu = "PowerPC 620";
			break;
		}
		case CPU_SUBTYPE_POWERPC_7450: {
			PLAT_Info.cpu = "PowerPC 7450 \"G4\"";
			break;
		}
		default: {
			printf("OSX: Unknown PPC Subtype 0x%08X\n", cpuSubType);
			break;
		}
	}
#else
	size_t length = sizeof(PLAT_Info.cpu);
	if (sysctlbyname("machdep.cpu.brand_string", &PLAT_Info.cpu, &length, NULL, 0)) {
		perror("sysctlbyname for machdep.cpu.brand_string failed");
		return -ENOSYS;
	}
#endif
	return 0;
}
