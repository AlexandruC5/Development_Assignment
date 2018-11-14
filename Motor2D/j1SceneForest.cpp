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
#include "j1EntityManager.h"
#include "j1SceneDesert.h"
#include "j1SceneForest.h"
#include "j1Collision.h"


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
	music_file = conf.child("music").child_value();
	bool ret = true;

	return ret;
}

// Called each loop iteration
bool j1SceneForest::Update(float dt)
{
	j1Scene::Update(dt);
	
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->swap_scene->FadeToBlack(this, App->scene_desert);
	}
	return true;
}

bool j1SceneForest::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == COLLIDER_PLAYER && App->entitymanager->player->state != WIN)
	{
		App->swap_scene->FadeToBlack(this, App->scene_desert);
		App->entitymanager->player->state = WIN;
	}
	return true;
}