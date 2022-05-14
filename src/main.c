#include <stdio.h>
#include <stdlib.h>
#include "pd_api.h"

#include "game.h"
#include "input.h"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* api, PDSystemEvent event, uint32_t arg)
{
	(void)arg;

	if (event == kEventInit)
	{
		const char* err;
		font = api->graphics->loadFont(fontpath, &err);

		if (font == NULL)
			api->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		api->system->setUpdateCallback(update, api);
	}

	return 0;
}


static int update(void* userdata)
{
	// Assign global API pointer
	pd = userdata;

	// Poll input
	input.Poll();

	// Run game state
	STATE_RUN(game.state);

	return 1;
}

