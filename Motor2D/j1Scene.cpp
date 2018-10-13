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
		App->render->camera.body.y -= 10;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.body.y += 10;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.body.x -= 10;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.body.x += 10;

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
		App->render->camera.target_speed.x = -(App->player->velocity.x);
	}
	else
	{
		if (App->player->position.x < LEFT_CAMERA_LIMIT && App->player->velocity.x < 0.0F)
		{
			App->render->camera.target_speed.x = -(App->player->velocity.x);
		}
		else
		{
			App->render->camera.target_speed.x = 0.0F;
		}
	}

	if (App->player->position.y < TOP_CAMERA_LIMIT && App->player->velocity.y < 0.0F)
	{
		App->render->camera.target_speed.y = -(App->player->velocity.y);
	}
	else
	{
		if (App->player->position.y + App->player->collider->rect.h > BOTTOM_CAMERA_LIMIT && App->player->velocity.y > 0.0F)
		{
			App->render->camera.target_speed.y = -(App->player->velocity.y);
		}
		else
		{
			App->render->camera.target_speed.y = 0.0F;
		}

	}

	App->render->camera.velocity = App->render->camera.target_speed * 0.3F + App->render->camera.velocity * (1 - 0.3F);
	if (fabs(App->render->camera.velocity.y) < App->player->threshold) App->render->camera.velocity.y = 0.0F;
	if (fabs(App->render->camera.velocity.x) < App->player->threshold) App->render->camera.velocity.x = 0.0F;
	App->render->camera.body.y += App->render->camera.velocity.y;
	App->render->camera.body.x += App->render->camera.velocity.x;

	if (-(App->render->camera.body.y - App->render->camera.body.h) > App->map->data.height * App->map->data.tile_height)
		App->render->camera.body.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.body.h);
	else if (-App->render->camera.body.y < 0)
		App->render->camera.body.y = 0;
	if (-App->render->camera.body.x < 0)
		App->render->camera.body.x = 0;
	else if (-App->render->camera.body.x > App->map->data.width*App->map->data.tile_width)
		App->render->camera.body.x = (-App->map->data.width*App->map->data.tile_width) - App->render->camera.body.w;

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