#include "coregame.h"

#include "input.h"
#include "game.h"
#include "resources.h"

#define _USE_MATH_DEFINES
#include <math.h>

float smoothCrank = 0.0f;
const float LPF_BETA = 0.08f; // 0<ß<1
const float CRANK_MAX = 45.0f;

const int MAIN_COLUMN_WIDTH = 240;
const int PILLARBOX_WIDTH = (LCD_COLUMNS - 240) >> 1;

const int PILLARBOX_L_LEFT = 0;
const int MAIN_COLUMN_LEFT = 80;
const int PILLARBOX_R_LEFT = 80 + 240;

const int CAM_INITIAL_HEIGHT = LCD_ROWS >> 2;
const int SHIP_WIDTH = 32;
const int SHIP_HEIGHT = 48;
const float SHIP_ROTATE_SPEED = M_PI * 2.0f / 120.0f;

const float JET_ACC = 0.5f;
const float GRAVITY = 0.15f;

static void CoreGame_Init()
{
	smoothCrank = 0.0f;

	coreGame.scoreHeight = 0;
	coreGame.camHeight = CAM_INITIAL_HEIGHT;
	coreGame.onGround = 1;
	coreGame.jetPower = 0.0f;
	coreGame.localX = 0.0f;
	coreGame.localY = (float)CAM_INITIAL_HEIGHT;
	coreGame.localY -= (float)(SHIP_HEIGHT >> 1);
	coreGame.lastLocalY = coreGame.localY;
	coreGame.gainLineY = coreGame.localY;
	coreGame.accumulator = 0.0f;
	coreGame.angle = M_PI * 1.5f;
	coreGame.dx = 0.0f;
	coreGame.dy = 0.0f;
	coreGame.scoreHeight = 0;
	coreGame.dead = 0;
}

#pragma region Helper

static inline void PointRotate(float inX, float inY, float angle, float* outX, float* outY)
{
	*outX = inX * cosf(angle) - inY * sinf(angle);
	*outY = inY * cosf(angle) + inX * sinf(angle);
}

static inline void PointPivot(float inX, float inY, float pivotX, float pivotY, float angle, float* outX, float* outY)
{
	inX -= pivotX;
	inY -= pivotY;
	PointRotate(inX, inY, angle, outX, outY);
	*outX += pivotX;
	*outY += pivotY;
}

static inline float AddAngle(float inAngle, float angleDelta)
{
	float outAngle = inAngle + angleDelta;
	while (outAngle > M_PI * 2.0f)
		outAngle -= M_PI * 2.0f;
	while (outAngle < 0.0f)
		outAngle += M_PI * 2.0f;
	return outAngle;
}

#pragma endregion

#pragma region Update

static void CalcJetPower()
{
	smoothCrank = smoothCrank - (LPF_BETA * (smoothCrank - fabsf(input.crankChange)));

	float power = 0.0f;
	power = MAX(0.0f, MIN(smoothCrank, CRANK_MAX)) / CRANK_MAX;
	power *= 8.0f;
	power = roundf(power);
	power /= 8.0f;
	coreGame.jetPower = power;
}

static void MoveShip()
{
	float mag = JET_ACC * coreGame.jetPower;
	float accX = mag * cosf(coreGame.angle);
	float accY = mag * sinf(coreGame.angle);

	if (coreGame.onGround)
	{
		if (mag > 0.0f)
			coreGame.onGround = 0;
	}

	if (coreGame.onGround == 0)
	{
		coreGame.dx += accX;
		coreGame.dy += accY;
		coreGame.dy += GRAVITY;

		coreGame.localX += coreGame.dx;
		coreGame.localY += coreGame.dy;
	}

	float halfWidth = (float)(MAIN_COLUMN_WIDTH >> 1);
	float fullWidth = (float)(MAIN_COLUMN_WIDTH);
	if (coreGame.localX < -halfWidth)
		coreGame.localX += fullWidth;
	else if (coreGame.localX > halfWidth)
		coreGame.localX -= fullWidth;

	if (input.buttonHold & kButtonLeft)
		coreGame.angle = AddAngle(coreGame.angle, -SHIP_ROTATE_SPEED);
	else if (input.buttonHold & kButtonRight)
		coreGame.angle = AddAngle(coreGame.angle, SHIP_ROTATE_SPEED);
}

static void TrackCamera()
{
	if (coreGame.localY < coreGame.gainLineY)
	{
		coreGame.accumulator = coreGame.gainLineY - coreGame.localY;

		while (coreGame.accumulator > 1.0f)
		{
			coreGame.scoreHeight++;
			coreGame.accumulator -= 1.0f;
		}

		coreGame.gainLineY = MAX(coreGame.localY, 0.0f);
	}

	if (coreGame.localY < 0.0f)
		coreGame.localY = 0.0f;

	if (coreGame.scoreHeight + (SHIP_HEIGHT >> 1) > coreGame.camHeight)
		coreGame.camHeight = coreGame.scoreHeight + (SHIP_HEIGHT >> 1);

	if (coreGame.localY > (LCD_ROWS >> 1) + (SHIP_HEIGHT >> 1))
		coreGame.dead = 1;
}

char crankBuf[32] = { '\0' };
static void CoreGame_Update()
{
	CalcJetPower();

	MoveShip();

	TrackCamera();
}

#pragma endregion

#pragma region Draw

static void Draw_HUD()
{
	pd->graphics->fillRect(PILLARBOX_L_LEFT, 0, PILLARBOX_WIDTH, LCD_ROWS, kColorBlack);
	pd->graphics->fillRect(PILLARBOX_R_LEFT, 0, PILLARBOX_WIDTH, LCD_ROWS, kColorBlack);

	// pd->graphics->drawLine(MAIN_COLUMN_LEFT, (LCD_ROWS >> 1) + (int)coreGame.gainLineY, PILLARBOX_R_LEFT, (LCD_ROWS >> 1) + (int)coreGame.gainLineY, 1, kColorBlack);

	pd->graphics->drawLine(MAIN_COLUMN_LEFT, LCD_ROWS - 4, PILLARBOX_R_LEFT, LCD_ROWS - 4, 1, kColorBlack);
	pd->graphics->fillRect(MAIN_COLUMN_LEFT, LCD_ROWS - 4, (int)(MAIN_COLUMN_WIDTH * coreGame.jetPower), 16, kColorBlack);

	pd->graphics->fillRect(0, 0, 64, 32, kColorWhite);
	char score[16] = { 0 };
	sprintf(score, "%06d", coreGame.scoreHeight);
	pd->graphics->drawText(score, 6, kASCIIEncoding, 0, 0);
	sprintf(score, "%06d", coreGame.camHeight);
	pd->graphics->drawText(score, 6, kASCIIEncoding, 0, 16);
}

static void Draw_Ground()
{
	int y = LCD_ROWS >> 1;
	y += coreGame.camHeight;

	for (int i = 0; i < 5 && y < LCD_ROWS; i++, y += 2 * i)
		pd->graphics->drawLine(MAIN_COLUMN_LEFT, y, PILLARBOX_R_LEFT, y, 0, kColorBlack);
}

static void Draw_Ship()
{
	// Local height is ship pos relative to screen center
	// (camHeight)

	int cx = (int)(floorf(coreGame.localX)) + (LCD_COLUMNS >> 1);
	int cy = (int)(floorf(coreGame.localY)) + (LCD_ROWS >> 1);

	float lines[4][4] = {
		{
			-SHIP_WIDTH * 0.5f, -SHIP_HEIGHT * 0.5f,
			SHIP_WIDTH * 0.5f, -SHIP_HEIGHT * 0.5f
		},
		{
			SHIP_WIDTH * 0.5f, -SHIP_HEIGHT * 0.5f,
			SHIP_WIDTH * 0.5f, SHIP_HEIGHT * 0.5f
		},
		{
			SHIP_WIDTH * 0.5f, SHIP_HEIGHT * 0.5f,
			-SHIP_WIDTH * 0.5f, SHIP_HEIGHT * 0.5f
		},
		{
			-SHIP_WIDTH * 0.5f, SHIP_HEIGHT * 0.5f,
			-SHIP_WIDTH * 0.5f, -SHIP_HEIGHT * 0.5f
		}
	};

	int drawCount = 1;
	if (coreGame.localX < -(float)((MAIN_COLUMN_WIDTH >> 1) - 64))
	{
		drawCount++;
	}
	else if (coreGame.localX > (float)((MAIN_COLUMN_WIDTH >> 1) - 64))
	{
		drawCount++;
		cx -= MAIN_COLUMN_WIDTH;
	}

	float drawAngle = AddAngle(coreGame.angle, -M_PI * 1.5f);

	for (int drawTimes = 0; drawTimes < drawCount; drawTimes++, cx += MAIN_COLUMN_WIDTH)
	{
		for (int i = 0; i < 4; i++)
		{
			float startX = lines[i][0];
			float startY = lines[i][1];
			float endX = lines[i][2];
			float endY = lines[i][3];

			PointRotate(lines[i][0], lines[i][1], drawAngle, &startX, &startY);
			PointRotate(lines[i][2], lines[i][3], drawAngle, &endX, &endY);

			pd->graphics->drawLine((int)startX + cx, (int)startY + cy,
				(int)endX + cx, (int)endY + cy,
				0, kColorBlack);
		}
	}
 }

static void CoreGame_Draw()
{
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);

	if (coreGame.camHeight <= (LCD_ROWS >> 1))
		Draw_Ground();

	Draw_Ship();

	Draw_HUD();
}

#pragma endregion

CoreGame coreGame = {
	.Init = CoreGame_Init,
	.Update = CoreGame_Update,
	.Draw = CoreGame_Draw
};