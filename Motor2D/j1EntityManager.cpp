#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1FlyingEnemy.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"

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

bool j1EntityManager::PostUpdate()
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		entity->data->PostUpdate();
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

bool j1EntityManager::CleanMapEntities()
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		if (entity->data != player)
		{
			entity->data->CleanUp();
			entities.del(entity);
		}
	}
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

bool j1EntityManager::CreateEntity(EntityType type, fPoint position, int clone_number)
{
	BROFILER_CATEGORY("Create_Entity", Profiler::Color::MediumOrchid);
	p2SString id;
	switch (type)
	{
	case EntityType::PLAYER:
		id.create("%s_%i", "player", id_count++);
		player = new j1Player(type, entity_configs.child("player"), position, id, clone_number);
		entities.add(player);
		break;
	case EntityType::PLAYERCLONE:
		id.create("%s_%i", "player", id_count++);
		entities.add(new j1Player(type, entity_configs.child("player"), position, id, clone_number));
		break;
	case EntityType::ENEMY:
		id.create("%s_%i", "enemy", id_count++);
		entities.add(new j1Enemy(type, entity_configs.child("enemy"), position, id));
		break;
	case EntityType::FLIER:
		id.create("%s_%i", "flier", id_count++);
		entities.add(new j1FlyingEnemy(type, entity_configs.child("flying_enemy"), position, id));
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

bool j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		if (entity->data->collider && entity->data->collider == c1)
			entity->data->OnCollision(c1, c2);
		else if (entity->data->collider && entity->data->collider == c2)
			entity->data->OnCollision(c2, c1);
	}
	return true;
}

float j1EntityManager::Reagroup()
{
	float scale = 0;
	int reduction = 0;
	for (p2List_item<j1Entity*>* entity = entities.start; entity; entity = entity->next)
	{
		if (entity->data->GetType() == EntityType::PLAYER || entity->data->GetType() == EntityType::PLAYERCLONE)
		{
			reduction++;
			scale += entity->data->GetScale();
			if (entity->data->GetType() == EntityType::PLAYERCLONE)
			{
				entities.del(entity);
			}
		}
	}
	if (reduction >= scale)
	{
		scale = (reduction-1) * 0.2f;
		reduction = 0;
	}
	return scale-reduction;
}
