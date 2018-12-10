#ifndef __J1SWAPSCENE_H__
#define __J1SWAPSCENE_H__

#include "j1Module.h"


class j1SwapScene : public j1Module
{
public:
	j1SwapScene();
	~j1SwapScene();

	bool Start();
	bool PostUpdate();
	bool FadeToBlack(float time = 2.0f);
	bool CleanUp();

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	uint start_time = 0;
	uint total_time = 0;
	SDL_Rect screen;
};

#endif