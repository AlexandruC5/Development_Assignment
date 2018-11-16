#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1FlyingEnemy.h"
#include "j1EntityManager.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entity_manager");
	CreateEntity(EntityType::PLAYER);
}

j1EntityManager::~j1EntityManager()
{

}

bool j1EntityManager::Awake(pugi::xml_node &conf)
{
	player->Awake(conf.child("player"));
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

	return true;
}

bool j1EntityManager::CreateEntity(EntityType type)
{
	switch (type)
	{
	case EntityType::PLAYER:
		player = new j1Player(type);
		entities.add(player);
		break;
	case EntityType::ENEMY:
		entities.add(new j1Enemy(type))->data->Start();
		break;
	case EntityType::FLIER:
		entities.add(new j1FlyingEnemy(type))->data->Start();
		break;
	}
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
