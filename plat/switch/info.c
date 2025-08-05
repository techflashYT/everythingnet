/*
 * EverythingNet - Switch Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>
#include <switch/services/sm.h>
#include <switch/services/set.h>

#include <evrnet/node.h>
#include <evrnet/cap.h>
#include <evrnet/plat.h>
#include <evrnet/state.h>

#define CPU_ERISTA "NVIDIA Tegra X1 - T210 - Erista"
#define CPU_MARIKO "NVIDIA Tegra X1+ - T210B01 - Mariko"
#define CPU_UNK    "NVIDIA Tegra X1 - Unknown Revision"
#define GPU_ERISTA "NVIDIA Tegra X1 (NVIDIA Maxwell GM20B)"
#define GPU_MARIKO "NVIDIA Tegra X1+ (NVIDIA Maxwell GM21B)"
#define GPU_UNK    "NVIDIA Tegra X1 (NVIDIA Maxwell GM2xx, Unknown Revision)"

platInfo_t PLAT_Info = {
	.arch  = PLAT_STR_ARMV80,
	.cap   = CAP_DISP |
		 CAP_SND |
		 CAP_GFX_ACCEL |
		 CAP_INPUT |
		 CAP_INPUT_REL |
		 CAP_INPUT_ABS
};

int SWITCH_GatherInfo() {
	uint64_t memSz;
	SetSysFirmwareVersion hosVer;
	SetSysProductModel model;
	SetSysDeviceNickName nick;
	Result result;

	/* get memory size */
	svcGetInfo(&memSz, InfoType_TotalMemorySize, CUR_PROCESS_HANDLE, 0);
	PLAT_Info.memSz = memSz / 1024;

	/* get Horizon OS version */
	result = setsysGetFirmwareVersion(&hosVer);

	/* set version str */
	PLAT_Info.os = malloc(128);
	if (!PLAT_Info.os) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	strcpy(PLAT_Info.os, "Nintendo Horizon OS ");
	if (R_SUCCEEDED(result))
		strcat(PLAT_Info.os, hosVer.display_version);
	else
		strcat(PLAT_Info.os, "(Unknown Version)");

	/* get console type */
	result = setsysGetProductModel(&model);

	if (!R_SUCCEEDED(result)) {
		PLAT_Info.name = "Unknown Nintendo Switch Model";
		PLAT_Info.cpu  = CPU_UNK;
		PLAT_Info.gpu  = GPU_UNK;
		puts("Failed to get Switch console type");
		goto noModel;
	}

	switch (model) {
		case SetSysProductModel_Nx:
			PLAT_Info.name = "Nintendo Switch (v1, 2017)";
			PLAT_Info.cpu  = CPU_ERISTA;
			PLAT_Info.gpu  = GPU_ERISTA;
			break;
		case SetSysProductModel_Iowa:
			PLAT_Info.name = "Nintendo Switch (v2, 2019)";
			PLAT_Info.cpu  = CPU_MARIKO;
			PLAT_Info.gpu  = GPU_MARIKO;
			break;
		case SetSysProductModel_Hoag:
			PLAT_Info.name = "Nintendo Switch Lite (v2, 2019)";
			PLAT_Info.cpu  = CPU_MARIKO;
			PLAT_Info.gpu  = GPU_MARIKO;
			break;
		case SetSysProductModel_Aula:
			PLAT_Info.name = "Nintendo Switch OLED (v2, 2019)";
			PLAT_Info.cpu  = CPU_MARIKO;
			PLAT_Info.gpu  = GPU_MARIKO;
			break;
		default:
			PLAT_Info.name = "Unknown Nintendo Switch Model";
			PLAT_Info.cpu  = CPU_UNK;
			PLAT_Info.gpu  = GPU_UNK;
			break;
	}
noModel:

	result = setsysGetDeviceNickname(&nick);
	if (!R_SUCCEEDED(result)) {
		strcpy(NODE_LocalName, "Unknown");
		puts("Failed to get Switch nickname");
	}

	strcpy(NODE_LocalName, nick.nickname);

	return 0;
}
