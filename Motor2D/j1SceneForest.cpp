#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1SwapScene.h"
#include "j1Map.h"
#include "j1Scene2.h"
#include "j1SceneForest.h"


j1SceneForest::j1SceneForest() : j1Scene()
{
	name.create("scene_forest");
}

// Destructor
j1SceneForest::~j1SceneForest()
{}

// Called before render is available
bool j1SceneForest::Awake(pugi::xml_node& conf)
{
	LOG("Loading Scene");
	map_file = conf.child("map_file").child_value();
	background_file = conf.child("background").child_value();
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1SceneForest::Start()
{
	App->map->Load(map_file.GetString());
	return true;
}

// Called each loop iteration
bool j1SceneForest::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 1;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 1;
	
	j1Scene::Update(dt);
	
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		App->swap_scene->FadeToBlack(this, App->scene2);
	}
	return true;
}

bool j1SceneForest::CleanUp()
{
	return true;
}