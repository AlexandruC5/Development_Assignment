#include "j1App.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1SceneForest.h"
#include "j1SceneDesert.h"
#include "j1Map.h"
#include "j1SwapScene.h"

j1SwapScene::j1SwapScene()
{
	name.create("swap_scene");
}

j1SwapScene::~j1SwapScene()
{}

// Load assets
bool j1SwapScene::Start()
{
	LOG("Preparing Fade Screen");
	uint width = 0U;
	uint height = 0U;
	App->win->GetWindowSize(width, height);
	screen = { 0, 0, (int)(width * App->win->GetScale()), (int)(height * App->win->GetScale()) };

	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1SwapScene::PostUpdate()
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			scene_disable->Deactivate();
			scene_enable->Activate();

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0F - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	default:
		break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0F));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1SwapScene::FadeToBlack(j1Scene* scene_off, j1Scene* scene_on, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5F * 1000.0F);
		scene_disable = scene_off;
		scene_enable = scene_on;
		ret = true;
	}

	return ret;
}

bool j1SwapScene::CleanUp()
{
	scene_disable = nullptr;
	scene_enable = nullptr;
	current_scene = nullptr;

	return true;
}

bool j1SwapScene::Load(pugi::xml_node &node)
{
	p2SString scene_name = node.child("scene").attribute("value").as_string();
	if (scene_name != current_scene->name)
	{
		current_scene->Deactivate();
		if (scene_name == "scene_forest") App->scene_forest->Activate();
		else App->scene_desert->Activate();
	}
	return true;
}

bool j1SwapScene::Save(pugi::xml_node &node) const
{
	pugi::xml_node scene_node;
	scene_node = node.append_child("scene");
	scene_node.append_attribute("value") = current_scene->name.GetString();
	return true;
}

void j1SwapScene::Reload()
{
	FadeToBlack(current_scene, current_scene);
}