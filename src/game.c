#include "game.h"
#include "resources.h"
#include "input.h"

#include "coregame.h"

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400 - TEXT_WIDTH) / 2;
int y = (240 - TEXT_HEIGHT) / 2;

int timer = 0;

static void State_Init();
static void State_Title();
static void State_GetReady();
static void State_Play();
static void State_Death();
static void State_HighScores();

static void State_Init()
{
	// TODO: Game init?

	game.state = State_Title;
	STATE_RUN(game.state);
}

static void State_Title()
{
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Liftoff!", strlen("Liftoff!"), kASCIIEncoding, x, y);
	pd->graphics->drawText("Press A", strlen("Press A"), kASCIIEncoding, x, y + TEXT_HEIGHT * 2);

	if (input.buttonPress & kButtonA)
	{
		game.state = State_GetReady;
		timer = 0;
	}
}

static void State_GetReady()
{
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Get Ready...", strlen("Get Ready"), kASCIIEncoding, x, y);

	if (timer < 30)
	{
		timer++;
	}
	else
	{
		coreGame.Init();
		game.state = State_Play;
	}
}

static void State_Play()
{
	coreGame.Update();
	coreGame.Draw();
}

static void State_Death()
{

}

static void State_HighScores()
{

}

Game game = {
	.screenHeight = LCD_ROWS,
	.screenWidth = LCD_COLUMNS,
	.state = State_Init
};

PlaydateAPI* pd = NULL;
