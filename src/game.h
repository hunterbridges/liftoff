#pragma once

#include "types.h"

typedef struct Game
{
	int32_t screenWidth;
	int32_t screenHeight;

	State state;
} Game;

extern Game game;
extern PlaydateAPI* pd;

