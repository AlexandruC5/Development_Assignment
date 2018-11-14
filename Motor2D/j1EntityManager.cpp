#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1EntityManager.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	player = new j1Player(EntityType::PLAYER);
}

j1EntityManager::~j1EntityManager()
{
}

bool j1EntityManager::Awake(pugi::xml_node &conf)
{
	player->Awake(conf);
	return false;
}

bool j1EntityManager::Start()
{
	player->Start();
	return false;
}

bool j1EntityManager::Update(float dt)
{
	player->Update(dt);
	return false;
}

bool j1EntityManager::PreUpdate()
{
	player->PreUpdate();
	return false;
}

bool j1EntityManager::CreateEntity(EntityType type)
{
	return false;
}

j1Entity * j1EntityManager::getEntity(EntityType type)
{
	return nullptr;
}

bool j1EntityManager::DeleteEntity(j1Entity * entity)
{
	return false;
}
