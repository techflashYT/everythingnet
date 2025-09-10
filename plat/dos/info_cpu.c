/*
 * EverythingNet - DOS Platform Specific Features - Info Gathering - CPU
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include <evrnet/plat/dos.h>
#include <evrnet/plat.h>


enum cpuModels {
	CPU_MODEL_386 = 0,
	CPU_MODEL_486,
	CPU_MODEL_PENTIUM,
	CPU_MODEL_PENTIUM_MMX,
	CPU_MODEL_PENTIUM_PRO /* or higher */
};

enum cpuTypes {
	CPU_TYPE_UNK = -1,
	CPU_TYPE_INTEL = 0,
	CPU_TYPE_AMD,
	/* TODO: Others? */
};

union cpuidStr {
	struct {
		uint32_t b;
		uint32_t d;
		uint32_t c;
	} u32;
	char str[12];
};

static jmp_buf env;


const char *strsAMD[] = {
	"AMD Am386",
	"AMD Am486/Am5x86",
	"AMD K5",
	"AMD K6",
	"AMD K7/Athlon or Later",
};

const char *strsIntel[] = {
	"Intel 80386",
	"Intel 80486",
	"Intel Pentium",
	"Intel Pentium MMX",
	"Intel Pentium Pro or later",
};

const char *strsIntelCompat[] = {
	"Intel 80386 or compatible",
	"Intel 80486 or compatible",
	"Intel Pentium or compatible",
	"Intel Pentium MMX or compatible",
	"Intel Pentium Pro or later",
};


/* Abuse the AC bit in eflags to tell if we have at least a 486 */
int isAtLeast486(void) {
	uint32_t oldf, newf;
	__asm__ __volatile__ (
		"pushfl\n\t"
		"popl %0\n\t"            /* %0 = oldf */
		"movl %0, %1\n\t"
		"xorl $0x00040000, %1\n\t" /* flip AC */
		"pushl %1\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %1\n\t"            /* %1 = newf */
		: "=&r"(oldf), "=&r"(newf) : : "memory"
	);
	return ((oldf ^ newf) & 0x00040000) != 0;
}


static void sigillHandlerTest(int signo) {
	(void)signo;
	/* longjmp back */
	longjmp(env, 1);
	return;
}

void DOS_GatherCPUInfo(void) {
	enum cpuModels cpuModel = CPU_MODEL_386;
	enum cpuTypes  cpuType  = CPU_TYPE_UNK;
	union cpuidStr mfg;
	uint32_t code = 1, eax = 0, ebx = 0, ecx = 0, edx = 0, family, model, stepping;
	void (*orig)(int); /* original handler */
	(void)stepping;

	mfg.u32.b = 0;
	mfg.u32.d = 0;
	mfg.u32.c = 0;

	/* install SIGINT handler for this test */
	orig = signal(SIGILL, sigillHandlerTest);

	/* is this a 386? */
	if (!isAtLeast486()) {
		/* must be a 386 */
		cpuModel = CPU_MODEL_386;
		goto out;
	}
	/* it's at least a 486 - check if we can run CPUID */

	if (setjmp(env) == 0) {
		/* try to CPUID */
		asm volatile(
			"cpuid"
			: "=a"(eax), "=d"(edx) : "a"(code) : "ecx", "ebx"
		);
	}
	else {
		/*
		 * faulted - since we ensured that we get here with at least a 486,
		 * then we must have a 486 w/o CPUID
		 * (Pentium and later always support CPUID).
		 */
		cpuModel = CPU_MODEL_486;
		goto out;
	}

	/* did not fault, parse it */
	/*printf("CPUID: eax: 0x%08lX,  edx: 0x%08lX\n", eax, edx);*/
	family = (eax & 0x00000F00) >> 8;
	model  = (eax & 0x000000F0) >> 4;
	stepping  = (eax & 0x0000000F);
	printf("Family %ld, model %ld, stepping %ld\n", family, model, stepping);
	/* must be at least 486 */
	cpuModel = CPU_MODEL_486;

	/* Pentium? */
	if (family == 5)
		cpuModel = CPU_MODEL_PENTIUM;

	/* PMMX? */
	if (family == 5 && model == 4)
		cpuModel = CPU_MODEL_PENTIUM_MMX;

	/* AMD K6 (has MMX) */
	if (family == 5 && model == 6)
		cpuModel = CPU_MODEL_PENTIUM_MMX;

	/* PPro (or better)? */
	if (family >= 6)
		cpuModel = CPU_MODEL_PENTIUM_PRO;

	code = 0; /* get manufacturer info */
	asm volatile(
		"cpuid"
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(code)
	);
	mfg.u32.b = ebx;
	mfg.u32.d = edx;
	mfg.u32.c = ecx;
	
	if (memcmp(mfg.str, "GenuineIntel", 12) == 0)
		cpuType = CPU_TYPE_INTEL;
	else if (memcmp(mfg.str, "AuthenticAMD", 12) == 0)
		cpuType = CPU_TYPE_AMD;
	else
		cpuType = CPU_TYPE_UNK;

out:
	/* restore SIGILL handler */
	signal(SIGILL, orig);

	/*printf("cpuModel: %d86\n", cpuModel + 3);*/
	switch (cpuType) {
		case CPU_TYPE_INTEL: {
			PLAT_Info.cpu = (char *)strsIntel[cpuModel];
			break;
		}
		case CPU_TYPE_AMD: {
			PLAT_Info.cpu = (char *)strsAMD[cpuModel];
			break;
		}
		default: {
			PLAT_Info.cpu = (char *)strsIntelCompat[cpuModel];
			break;
		}
	}
	return;
}

