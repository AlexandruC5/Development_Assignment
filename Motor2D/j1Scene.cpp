#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1SceneForest.h"
#include "j1SwapScene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "j1Enemy.h"

#define RIGHT_CAMERA_LIMIT  (-(App->render->camera.body.x - App->render->camera.body.w / 2))
#define LEFT_CAMERA_LIMIT  (-(App->render->camera.body.x - App->render->camera.body.w / 6))
#define TOP_CAMERA_LIMIT  (-(App->render->camera.body.y - App->render->camera.body.h / 8))
#define BOTTOM_CAMERA_LIMIT (-(App->render->camera.body.y - App->render->camera.body.h / 2))

j1Scene::j1Scene() : j1Module()
{}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& conf)
{
	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->swap_scene->current_scene = this;
	App->entitymanager->player->ResetPlayer();
	App->map->Load(map_file.GetString());

	//pathfinding debug
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);

	RELEASE_ARRAY(data);
	debug_tex = App->tex->Load("maps/path.png");

	App->audio->PlayMusic(music_file.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p, 5,5,2);

			const p2DynArray<iPoint>* tmp_array = App->pathfinding->GetLastPath();
			//App->enemy->current_path.Clear();
			for (int i = 0; i < tmp_array->Count() ; i++)
			{		
				iPoint p = App->map->MapToWorld(tmp_array->At(i)->x, tmp_array->At(i)->y);
				p.x += App->map->data.tile_width / 2;
				p.y += App->map->data.tile_height / 2;
				//App->enemy->current_path.PushBack(p);
			}
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Debug Functionalities
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->swap_scene->FadeToBlack(App->swap_scene->current_scene, App->scene_forest);
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->swap_scene->Reload();
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();
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

	dt_ = dt;
	App->map->Draw();


	// Debug pathfinding ------------------------------
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	//Camera movement
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

	App->render->camera.velocity = ((App->render->camera.target_speed * 0.3F) + (App->render->camera.velocity * (1 - 0.3F)));
	if (fabs(App->render->camera.velocity.y) < App->entitymanager->player->threshold*dt_) App->render->camera.velocity.y = 0.0F;
	if (fabs(App->render->camera.velocity.x) < App->entitymanager->player->threshold*dt_) App->render->camera.velocity.x = 0.0F;
	App->render->camera.body.y += App->render->camera.velocity.y;
	App->render->camera.body.x += App->render->camera.velocity.x;

	//Check if camera out of bounds and place it on right spot
	if (-(App->render->camera.body.y - App->render->camera.body.h) > App->map->data.height * App->map->data.tile_height)
		App->render->camera.body.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.body.h);
	else if (-App->render->camera.body.y < 0)
		App->render->camera.body.y = 0;
	if (-App->render->camera.body.x < 0)
		App->render->camera.body.x = 0;
	else if (-(App->render->camera.body.x - App->render->camera.body.w) > App->map->data.width*App->map->data.tile_width)
		App->render->camera.body.x = -(App->map->data.width * App->map->data.tile_width - App->render->camera.body.w);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanUp();
	return true;
}