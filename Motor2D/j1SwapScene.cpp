#include "j1App.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1SwapScene.h"
#include "j1EntityManager.h"
#include "j1Gui.h"

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

	if (swap == FADE)
	{
		Uint32 now = SDL_GetTicks() - start_time;
		float normalized = MIN(1.0f, (float)now / (float)total_time);

		switch (current_step)
		{
		case fade_step::fade_to_black:
		{
			if (now >= total_time)
			{
				App->scene->LoadLevel();

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
		SDL_RenderSetClipRect(App->render->renderer, nullptr);
		SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0F));
		SDL_RenderFillRect(App->render->renderer, &screen);
	}
	else
	{
		Uint32 now = SDL_GetTicks() - start_time;
		float normalized = MIN(1.0f, (float)now / (float)total_time);

		if (now >= total_time)
		{
			App->scene->LoadLevel(save_game);
			App->gui->DisableElement(App->scene->loading_background);
			//end
			current_step = fade_step::none;
		}
	}
	

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1SwapScene::FadeToBlack(float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5F * 1000.0F);
		swap = FADE;
		ret = true;
	}

	return ret;
}

bool j1SwapScene::LoadScreen(float time, bool save_game)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		App->gui->EnableElement(App->scene->loading_background);
		App->gui->DisableElement(App->scene->menu_background);
		App->gui->DisableElement(App->scene->settings_menu_panel);
		App->gui->DisableElement(App->scene->credits_menu_panel);
		App->gui->DisableElement(App->scene->ingame_panel);

		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5F * 1000.0F);
		swap = LOAD_SCREEN;
		App->paused = false;
		this->save_game = save_game;
		ret = true;
		App->scene->saved_time = 0;
		App->entitymanager->player->ResetLives();
		App->scene->time_text->SetColor({ 0, 0, 0 });
		App->scene->score_text->SetColor({ 255,0,0 });
		App->entitymanager->player->score = 0;
		App->entitymanager->player->ResetScale();
	}

	return ret;
}

bool j1SwapScene::CleanUp()
{
	return true;
}