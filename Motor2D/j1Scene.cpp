#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1SwapScene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "j1Gui.h"
#include "j1Enemy.h"
#include "Brofiler/Brofiler.h"

#define RIGHT_CAMERA_LIMIT  (-(App->render->camera.position.x - App->render->camera.body.w / 2))
#define LEFT_CAMERA_LIMIT  (-(App->render->camera.position.x - App->render->camera.body.w / 6))
#define TOP_CAMERA_LIMIT  (-(App->render->camera.position.y - App->render->camera.body.h / 8))
#define BOTTOM_CAMERA_LIMIT (-(App->render->camera.position.y - App->render->camera.body.h / 2))

j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& conf)
{
	pugi::xml_node level;
	for (level = conf.child("levels").child("level"); level; level = level.next_sibling("level"))
	{
		Level lvl;
		lvl.map_path = level.child_value();
		lvl.sound_path = level.attribute("music").as_string();
		lvl.next_level = level.attribute("next_level").as_int() - 1;
		lvl.game_level = level.attribute("game_level").as_bool(true);
		levels.add(lvl);
	}
	current_level = conf.child("start_level").attribute("value").as_int() - 1;

	menu_background = App->gui->CreateImage({ 0,0 }, { 2371,1452,1280,720 });

	main_menu_panel = App->gui->CreateImage({ 850,50 }, { 551,711,380,539 }, menu_background);
	App->gui->ScaleElement(main_menu_panel, 0.0F, 0.17F);
	main_menu_button_play = App->gui->CreateButton({ 100, 75 }, main_menu_panel);
	main_menu_button_continue = App->gui->CreateButton({ 100, 175 }, main_menu_panel);
	main_menu_button_settings = App->gui->CreateButton({ 100, 275 }, main_menu_panel);
	main_menu_button_settings->dragable = true;
	main_menu_button_credits = App->gui->CreateButton({ 100, 375 }, main_menu_panel);
	main_menu_button_exit = App->gui->CreateButton({ 100, 475}, main_menu_panel);



	main_menu_button_play_text = App->gui->CreateLabel({ 58,22 },"fonts/open_sans/OpenSans-Bold.ttf", 28, "PLAY", { 255,255,255 }, 0, main_menu_button_play);
	main_menu_button_continue_text = App->gui->CreateLabel({ 17,22 }, "fonts/open_sans/OpenSans-Bold.ttf", 28, "CONTINUE", { 255,255,255 }, 0, main_menu_button_continue);
	main_menu_button_settings_text = App->gui->CreateLabel({ 25,22 }, "fonts/open_sans/OpenSans-Bold.ttf", 28, "SETTINGS", { 255,255,255 }, 0, main_menu_button_settings);
	main_menu_button_credits_text = App->gui->CreateLabel({ 35,22 }, "fonts/open_sans/OpenSans-Bold.ttf", 28, "CREDITS", { 255,255,255 }, 0, main_menu_button_credits);
	main_menu_button_exit_text = App->gui->CreateLabel({ 60,22 }, "fonts/open_sans/OpenSans-Bold.ttf", 28, "EXIT", { 255,255,255 }, 0, main_menu_button_exit);


	settings_menu_panel = App->gui->CreateImage({ 450,50 }, { 551,711,380,539 });
	App->gui->ScaleElement(settings_menu_panel, 0.0F, -0.4F);
	settings_menu_button_main_menu = App->gui->CreateButton({ 100, 180 }, settings_menu_panel);
	settings_menu_button_main_menu_text = App->gui->CreateLabel({ 60,14 }, "fonts/open_sans/OpenSans-Bold.ttf", 22, "MAIN\nMENU", { 255,255,255 }, 100, settings_menu_button_main_menu);


	pause_menu_panel = App->gui->CreateImage({ 450,50 }, { 551,711,380,539 });
	App->gui->ScaleElement(pause_menu_panel, 0.0F, -0.1F);
	pause_menu_button_resume = App->gui->CreateButton({ 100, 75 }, pause_menu_panel);
	pause_menu_button_main_menu = App->gui->CreateButton({ 100, 175 }, pause_menu_panel);
	pause_menu_button_resume_text = App->gui->CreateLabel({ 48,26 }, "fonts/open_sans/OpenSans-Bold.ttf", 22, "RESUME", { 255,255,255 }, 0, pause_menu_button_resume);
	pause_menu_button_main_menu_text = App->gui->CreateLabel({ 60,14 }, "fonts/open_sans/OpenSans-Bold.ttf", 22, "MAIN\nMENU", { 255,255,255 }, 100, pause_menu_button_main_menu);

	credits_menu_panel = App->gui->CreateImage({ 450,50 }, { 551,711,380,539 });
	App->gui->ScaleElement(credits_menu_panel, 0.0F, 0.1F);
	credits_menu_text_panel = App->gui->CreateImage({ 60, 70 }, { 1172,2283,297,395 }, credits_menu_panel);
	App->gui->ScaleElement(credits_menu_text_panel, -0.12F, -0.1F);
	credits_menu_button_main_menu = App->gui->CreateButton({ 100, 400 }, credits_menu_panel);
	credits_menu_button_main_menu_text = App->gui->CreateLabel({ 60,14 }, "fonts/open_sans/OpenSans-Bold.ttf", 22, "MAIN\nMENU", { 255,255,255 }, 80, credits_menu_button_main_menu);
	credits_menu_text = App->gui->CreateLabel({ 5,5 }, "fonts/open_sans/OpenSans-Bold.ttf", 18, "MIT License\n\nCopyright(c) 2018 [Axel Alavedra Cabello, Alejandro París Gómez]\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the ""Software""), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.", { 255,255,255 }, 250, credits_menu_text_panel);
	credits_menu_text->parent_limit = false;
	scroll = App->gui->CreateScrollBar({ 335, 150 }, credits_menu_panel);



	App->gui->DisableElement(pause_menu_panel);
	App->gui->DisableElement(settings_menu_panel);
	App->gui->DisableElement(credits_menu_panel);

	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	BROFILER_CATEGORY("StartScene", Profiler::Color::Plum);
	App->entitymanager->CleanUp();

	App->map->Load(levels.At(current_level)->data.map_path.GetString());
	App->entitymanager->player->ResetEntity();

	//pathfinding
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);

	RELEASE_ARRAY(data);

	App->audio->PlayMusic(levels.At(current_level)->data.sound_path.GetString());

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Debug Functionalities
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		current_level = 0;
		App->swap_scene->FadeToBlack();
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->swap_scene->FadeToBlack();
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		current_level = levels.At(current_level)->data.next_level;
		App->swap_scene->FadeToBlack();
	}
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && App->frame_cap == true)
	{
		App->frame_cap = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		App->frame_cap = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) App->audio->IncreaseFXVolume();
		else App->audio->IncreaseMusicVolume();
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) App->audio->DecreaseFXVolume();
		else App->audio->DecreaseMusicVolume();
	}		
	if (levels.At(current_level)->data.game_level && App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (App->paused)
		{
			App->paused = false;
			App->gui->DisableElement(pause_menu_panel);
		}
		else
		{
			App->paused = true;
			App->gui->EnableElement(pause_menu_panel);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
		App->gui->ScaleElement(main_menu_panel, -0.5F, -0.5F, 0.5F);
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		App->gui->ScaleElement(main_menu_panel, 0.5F, 0.5F, 0.5F);


	
	App->render->camera.velocity = ((App->render->camera.target_speed * 0.4F) + (App->render->camera.velocity * (1 - 0.4F)));

	App->render->camera.position.y += App->render->camera.velocity.y;
	App->render->camera.position.x += App->render->camera.velocity.x;
	App->render->camera.body.x = App->render->camera.position.x;
	App->render->camera.body.y = App->render->camera.position.y;

	//Check if camera out of bounds and place it on right spot
	if (-(App->render->camera.position.y - App->render->camera.body.h) > App->map->data.height * App->map->data.tile_height)
		App->render->camera.position.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.body.h);
	else if (-App->render->camera.position.y < 0)
		App->render->camera.position.y = 0;
	if (-App->render->camera.position.x < 0)
		App->render->camera.position.x = 0;
	else if (-(App->render->camera.position.x - App->render->camera.body.w) > App->map->data.width*App->map->data.tile_width)
		App->render->camera.position.x = -(App->map->data.width * App->map->data.tile_width - App->render->camera.body.w);

	if(levels.At(current_level)->data.game_level) App->map->Draw();
	


	//DIRTY
	SDL_Rect local_rect = credits_menu_text->GetLocalRect();
	SDL_Rect parent_rect = credits_menu_text_panel->GetLocalRect();
	float scaleX, scaleY;
	credits_menu_text_panel->GetScale(scaleX, scaleY);

	iPoint local_pos = credits_menu_text->GetLocalPos();
	credits_menu_text->SetLocalPos(local_pos.x/scaleX, scroll->GetValue(5, 5 - (local_rect.h - parent_rect.h))/scaleY);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	//Camera movement
	if (levels.At(current_level)->data.game_level)
	{
		if (App->entitymanager->player->position.x + App->entitymanager->player->collider->rect.w > RIGHT_CAMERA_LIMIT && App->entitymanager->player->velocity.x > 0.0F)
		{
			App->render->camera.target_speed.x = -(App->entitymanager->player->velocity.x);
		}
		else
		{
			if (App->entitymanager->player->position.x < LEFT_CAMERA_LIMIT && App->entitymanager->player->velocity.x < 0.0F)
			{
				App->render->camera.target_speed.x = -(App->entitymanager->player->velocity.x);
			}
			else
			{
				App->render->camera.target_speed.x = 0.0F;
			}
		}
		if (App->entitymanager->player->position.y < TOP_CAMERA_LIMIT && App->entitymanager->player->velocity.y < 0.0F)
		{
			App->render->camera.target_speed.y = -(App->entitymanager->player->velocity.y);
		}
		else
		{
			if (App->entitymanager->player->position.y + App->entitymanager->player->collider->rect.h > BOTTOM_CAMERA_LIMIT && App->entitymanager->player->velocity.y > 0.0F)
			{
				App->render->camera.target_speed.y = -(App->entitymanager->player->velocity.y);
			}
			else
			{
				App->render->camera.target_speed.y = 0.0F;
			}

		}
	}
	

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanUp();
	return true;
}

bool j1Scene::OnCollision(Collider * c1, Collider * c2)
{
	App->entitymanager->player->state = WIN;

	current_level = levels.At(current_level)->data.next_level;
	App->swap_scene->FadeToBlack();

	return true;
}

bool j1Scene::Load(pugi::xml_node &node)
{
	int saved_level = node.child("current_level").attribute("value").as_int();
	if (saved_level != current_level)
	{
		current_level = saved_level;
		App->swap_scene->FadeToBlack(0.0F);
	}

	return true;
}

bool j1Scene::Save(pugi::xml_node &node) const
{
	pugi::xml_node scene_node;
	scene_node = node.append_child("current_scene");
	scene_node.append_attribute("value") = current_level;

	return true;
}

bool j1Scene::GUIEvent(j1UIElement * element, GUI_Event gui_event)
{
	switch (gui_event)
	{
		case MOUSE_OVER:
		{
			
		}
		break;
		case MOUSE_EXIT:
		{
		
		}
		break;
		case LEFT_CLICK_DOWN:
		{
			if (element == pause_menu_button_resume)
			{
				App->gui->DisableElement(pause_menu_panel);
				App->paused = false;
			}
			else if (element == settings_menu_button_main_menu)
			{
				App->gui->EnableElement(main_menu_panel);
				App->gui->DisableElement(settings_menu_panel);
			}
			else if(element == pause_menu_button_main_menu)
			{
				current_level = 0;
				App->swap_scene->FadeToBlack(0.0F);

				App->gui->EnableElement(menu_background);
				App->gui->DisableElement(pause_menu_panel);
			}
			else if(element == main_menu_button_play)
			{
				App->gui->DisableElement(menu_background);

				//? main menu should be a level or not?
				current_level = levels.At(current_level)->data.next_level;
				App->swap_scene->FadeToBlack(0.0F);

				App->paused = false;
			}
			else if(element == main_menu_button_continue)
			{
				App->gui->DisableElement(main_menu_panel);
				App->LoadGame();
				App->paused = false;
			}
			else if (element == main_menu_button_settings)
			{
				/*App->gui->EnableElement(settings_menu_panel);
				App->gui->DisableElement(main_menu_panel);*/
			}
			else if (element == main_menu_button_credits)
			{
				App->gui->EnableElement(credits_menu_panel);
				App->gui->DisableElement(main_menu_panel);
			}
			else if (element == main_menu_button_exit)
			{
				exit(0);
			}
			else if (element == credits_menu_button_main_menu)
			{
				App->gui->EnableElement(main_menu_panel);
				App->gui->DisableElement(credits_menu_panel);
			}
		}
		break;
	}

	return true;
}