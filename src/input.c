#include "input.h"
#include "game.h"

static void Input_Poll()
{
	pd->system->getButtonState(&input.buttonHold, &input.buttonPress, &input.buttonRelease);
	input.crankAngle = pd->system->getCrankAngle();
	input.crankChange = pd->system->getCrankChange();
}

Input input = {
	.buttonHold = 0,
	.buttonPress = 0,
	.buttonRelease = 0,
	.crankAngle = 0.0f,
	.crankChange = 0.0f,
	.Poll = Input_Poll
};