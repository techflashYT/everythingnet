#include <evrnet/plat.h>
#include <evrnet/cap.h>

platInfo_t PLAT_Info = {
	.name  = "IBM PC or Compatible",
	.os    = "MS-DOS or Compatible",
	.cpu   = "Intel 80386 or Compatible",
	.memSz = 640,
	.arch  = PLAT_STR_X86,
	.gpu   = "CGA or better (720x480 text mode)",
	.cap   = CAP_DISP      |
		 CAP_SND       |
		 CAP_INPUT     |
		 CAP_INPUT_ABS |
		 CAP_INPUT_REL
};
