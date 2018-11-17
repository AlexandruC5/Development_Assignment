#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1FlyingEnemy.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1EntityManager.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entity_manager");
}

j1EntityManager::~j1EntityManager()
{

}

bool j1EntityManager::Awake(pugi::xml_node &conf)
{
	entity_configs = conf;
	return true;
}

bool j1EntityManager::Start()
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->Start();
	}
	return true;
}

bool j1EntityManager::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) 
		draw_path = !draw_path;

	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->Update(dt);
	}
	return true;
}

bool j1EntityManager::PreUpdate()
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->PreUpdate();
	}
	return true;
}

bool j1EntityManager::CleanUp()
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->CleanUp();
	}
	entities.clear();
	player = nullptr;
	id_count = 0;

	return true;
}

bool j1EntityManager::Load(pugi::xml_node &file)
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->Load(file.child(entity->data->id.GetString()));
	}
	return true;
}

bool j1EntityManager::Save(pugi::xml_node &file) const
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->Save(file.append_child(entity->data->id.GetString()));
	}
	return true;
}

bool j1EntityManager::CreateEntity(EntityType type, fPoint position)
{
	char* count = (char*)malloc(sizeof(char) * 2);
	_itoa_s(id_count++, count, 2, 10);
	p2SString id;
	switch (type)
	{
	case EntityType::PLAYER:
		id = "player_";
		id += count;
		player = new j1Player(type, entity_configs.child("player"), position, id);
		entities.add(player);
		break;
	case EntityType::ENEMY:
		id = "enemy_";
		id += count;
		entities.add(new j1Enemy(type, entity_configs.child("enemy"), position, id));
		break;
	case EntityType::FLIER:
		id = "flier_";
		id += count;
		entities.add(new j1FlyingEnemy(type, entity_configs.child("flying_enemy"), position, id));
		break;
	}
	memset(count, 0, sizeof(char) * 2);
	return true;
}

j1Entity * j1EntityManager::getEntity(EntityType type)
{

	return nullptr;
}

bool j1EntityManager::DeleteEntity(j1Entity * entity)
{

	return true;
}
