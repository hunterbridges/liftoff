#pragma once

#include "types.h"

typedef struct CoreGame
{
	float jetPower;

	void(*Init)();
	void(*Update)();
	void(*Draw)();
} CoreGame;

extern CoreGame coreGame;
