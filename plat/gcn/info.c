/*
 * EverythingNet - GameCube Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <evrnet/plat.h>
#include <evrnet/cap.h>

platInfo_t PLAT_Info = {
	.name  = "Nintendo GameCube",
	.os    = "libogc",
	.arch  = PLAT_STR_PPC32,
	.cpu   = "IBM Gekko (1 core) @ 486MHz",
	.memSz = 24 * 1024,
	.gpu   = "ArtX \"GX\" (inside Flipper chipset) @ 162MHz",
	.cap   = CAP_DISP |
		 CAP_SND |
		 CAP_INPUT |
		 CAP_INPUT_REL
};
