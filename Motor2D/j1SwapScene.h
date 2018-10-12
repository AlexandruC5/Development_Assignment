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
	bool FadeToBlack(j1Scene* scene_off, j1Scene* scene_on, float time = 2.0f);
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	void Reload();

	j1Scene* current_scene = nullptr;
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
	j1Scene* scene_enable = nullptr;
	j1Scene* scene_disable = nullptr;
};

#endif