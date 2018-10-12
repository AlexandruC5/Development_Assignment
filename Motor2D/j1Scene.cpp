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
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Collision.h"

#define RIGHT_CAMERA_LIMIT  (-(App->render->camera.x - App->render->camera.w / 2))
#define LEFT_CAMERA_LIMIT  (-(App->render->camera.x - App->render->camera.w / 6))
#define TOP_CAMERA_LIMIT  (-(App->render->camera.y - App->render->camera.h / 6))
#define BOTTOM_CAMERA_LIMIT (-(App->render->camera.y - (App->render->camera.h - App->render->camera.h/6)))

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
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 10;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 10;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 10;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 10;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) App->swap_scene->FadeToBlack(App->swap_scene->current_scene, App->scene_forest);
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) App->swap_scene->Reload();

	App->map->Draw();

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count());

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->player->position.x + App->player->collider->rect.w > RIGHT_CAMERA_LIMIT && App->player->velocity.x > 0.0F)
	{
		App->render->camera.x -= App->player->velocity.x;
		if (-App->render->camera.x > App->map->data.width*App->map->data.tile_width)
			App->render->camera.x = (-App->map->data.width*App->map->data.tile_width) - App->render->camera.w;
	}
	if (App->player->position.x < LEFT_CAMERA_LIMIT && App->player->velocity.x < 0.0F)
	{
		App->render->camera.x += -App->player->velocity.x;
		if (-App->render->camera.x < 0)
			App->render->camera.x = 0;
	}
	if (App->player->position.y < TOP_CAMERA_LIMIT && App->player->velocity.y < 0.0F)
	{
		App->render->camera.y -= App->player->velocity.y;
		if (-App->render->camera.y < 0)
			App->render->camera.y = 0;
	}
	if (App->player->position.y + App->player->collider->rect.h > BOTTOM_CAMERA_LIMIT && App->player->velocity.y > 0.0F)
	{
		App->render->camera.y -= App->player->velocity.y;
		if (-(App->render->camera.y - App->render->camera.h) > App->map->data.height * App->map->data.tile_height)
			App->render->camera.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.h);
	}

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	int control_area = 0;
	if (App->player->position.x + App->player->collider->rect.w >= RIGHT_CAMERA_LIMIT && App->player->velocity.x > 0.0F)
	{
		velocity.x = (App->player->velocity.x*-1);
	}
	else
	{
		if (App->player->position.x <= LEFT_CAMERA_LIMIT && App->player->velocity.x < 0.0F)
		{
			velocity.x = (App->player->velocity.x*-1);
		}
		else
		{
			velocity.x = 0.0F;
		}
	}

	if (App->player->position.y <= TOP_CAMERA_LIMIT && App->player->velocity.y < 0.0F)
	{
		target_speed.y = 20;
	}
	else
	{
		if (App->player->position.y + App->player->collider->rect.h > BOTTOM_CAMERA_LIMIT && App->player->velocity.y > 0.0F)
		{
			target_speed.y = -20;
		}
		else
		{
			target_speed.y = 0.0F;
		}

	}

	velocity.y = target_speed.y * 0.3f + (App->player->velocity.y*-1);
	if (fabs(velocity.y) < App->player->threshold) velocity.y = 0.0F;
	if (fabs(velocity.x) < App->player->threshold) velocity.x = 0.0F;
	App->render->camera.y += velocity.y;
	App->render->camera.x += velocity.x;

	if (-(App->render->camera.y - App->render->camera.h) > App->map->data.height * App->map->data.tile_height)
		App->render->camera.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.h);
	if (-App->render->camera.y < 0)
		App->render->camera.y = 0;
	if (-App->render->camera.x < 0)
		App->render->camera.x = 0;
	if (-App->render->camera.x > App->map->data.width*App->map->data.tile_width)
		App->render->camera.x = (-App->map->data.width*App->map->data.tile_width) - App->render->camera.w;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanUp();
	return true;
}