/*
 * EverythingNet - Wii Platform Specific Features - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <evrnet/plat.h>
#include <evrnet/cap.h>

platInfo_t PLAT_Info = {
	.name  = "Nintendo Wii",
	.os    = "Nintendo/BroadOn IOS (Starlet), libogc (Broadway)",
	.arch  = PLAT_STR_PPC32,
	.cpu   = "IBM Broadway (1 core) @ 729MHz",
	.memSz = 88 * 1024,
	.gpu   = "ArtX \"GX\" (inside Hollywood chipset) @ 243MHz",
	.cap   = CAP_DISP |
		 CAP_SND |
		 CAP_INPUT |
		 CAP_INPUT_REL |
		 CAP_INPUT_ABS
};
