#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1SwapScene.h"
#include "j1SceneForest.h"
#include "j1Audio.h"
#include "j1Scene2.h"
#include "j1Collision.h"


j1Scene2::j1Scene2()
{
	name.create("scene2");
}

// Destructor
j1Scene2::~j1Scene2()
{}

// Called before render is available
bool j1Scene2::Awake(pugi::xml_node& conf)
{
	LOG("Loading Scene");
	map_file = conf.child("map_file").child_value();
	background_file = conf.child("background").child_value();
	music_file = conf.child("music").child_value();
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene2::Start()
{
	App->swap_scene->current_scene = this;
	App->map->Load(map_file.GetString());
	App->player->ResetPlayer();
	App->audio->PlayMusic(music_file.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene2::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

	j1Scene::Update(dt);

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) 
	{
		App->swap_scene->FadeToBlack(this, App->scene_forest);
	}

	return true;
}

bool j1Scene2::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == COLLIDER_PLAYER) App->swap_scene->FadeToBlack(this, App->scene_forest);
	return true;
}