#pragma once

#include "types.h"

typedef struct Input
{
	PDButtons buttonHold;
	PDButtons buttonPress;
	PDButtons buttonRelease;

	float crankAngle;
	float crankChange;

	void(*Poll)();
} Input;

extern Input input;
