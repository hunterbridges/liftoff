#include "coregame.h"

#include "input.h"
#include "game.h"
#include "resources.h"

float smoothCrank = 0.0f;
const float LPF_BETA = 0.08f; // 0<ß<1
const float CRANK_MAX = 45.0f;

const int MAIN_COLUMN_WIDTH = 240;
const int PILLARBOX_WIDTH = (LCD_COLUMNS - 240) >> 1;

const int PILLARBOX_L_LEFT = 0;
const int MAIN_COLUMN_LEFT = 80;
const int PILLARBOX_R_LEFT = 80 + 240;

static void CoreGame_Init()
{

}

#pragma region Update

static void CalcJetPower()
{
	smoothCrank = smoothCrank - (LPF_BETA * (smoothCrank - fabsf(input.crankChange)));

	float power = 0.0f;
	power = max(0.0f, min(smoothCrank, CRANK_MAX)) / CRANK_MAX;
	power *= 8.0f;
	power = roundf(power);
	power /= 8.0f;
	coreGame.jetPower = power;
}

char crankBuf[32] = { '\0' };
static void CoreGame_Update()
{
	CalcJetPower();
}

#pragma endregion

#pragma region Draw

static void CoreGame_Draw()
{
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Play Game", strlen("Play Game"), kASCIIEncoding, 200, 120);

	pd->graphics->fillRect(PILLARBOX_L_LEFT, 0, PILLARBOX_WIDTH, LCD_ROWS, kColorBlack);
	pd->graphics->fillRect(PILLARBOX_R_LEFT, 0, PILLARBOX_WIDTH, LCD_ROWS, kColorBlack);

	pd->graphics->drawLine(MAIN_COLUMN_LEFT, LCD_ROWS - 4, PILLARBOX_R_LEFT, LCD_ROWS - 4, 1, kColorBlack);
	pd->graphics->fillRect(MAIN_COLUMN_LEFT, LCD_ROWS - 4, (int)(MAIN_COLUMN_WIDTH * coreGame.jetPower), 16, kColorBlack);
}

#pragma endregion

CoreGame coreGame = {
	.Init = CoreGame_Init,
	.Update = CoreGame_Update,
	.Draw = CoreGame_Draw
};