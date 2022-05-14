#pragma once

#include "types.h"

typedef struct CoreGame
{
	float jetPower;

	uint32_t camHeight;
	uint32_t scoreHeight;

	uint8_t onGround;

	float localX;
	float localY;
	float lastLocalY;
	float gainLineY;
	float accumulator;

	float angle;
	float dx;
	float dy;

	int dead;

	void(*Init)();
	void(*Update)();
	void(*Draw)();
} CoreGame;

extern CoreGame coreGame;
