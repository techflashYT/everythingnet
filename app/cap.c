/*
 * EverythingNet - Platform capabilities support
 * Copyright (C) 2025 Techflash
 */
#include <string.h>

#include <evrnet/cap.h>
static void doAppend(char *out, const char *str, int *isFirst) {
	int _isFirst = *isFirst;
	if (!_isFirst) {
		strcat(out, " | ");
	}

	*isFirst = 0;
	strcat(out, str);
}

void CAP_Cap2Str(int cap, char *out) {
	int isFirst;
	isFirst = 1;

	if (!out)
		return;

	out[0] = '\0';

	if (cap & CAP_HOST)      doAppend(out, "Application Host", &isFirst);
	if (cap & CAP_DISP)      doAppend(out, "Display Other Apps", &isFirst);
	if (cap & CAP_GFX_ACCEL) doAppend(out, "Graphics Acceleration", &isFirst);
	if (cap & CAP_SND)       doAppend(out, "Sound Output", &isFirst);
	if (cap & CAP_INPUT)     doAppend(out, "Input", &isFirst);
	if (cap & CAP_INPUT_REL) doAppend(out, "Relative Input (Mouse, Joystick, etc)", &isFirst);
	if (cap & CAP_INPUT_ABS) doAppend(out, "Absolute Input (Touchscreen, Tablet, etc)", &isFirst);

	if (isFirst) {
		strcpy(out, "None");
	}
}
