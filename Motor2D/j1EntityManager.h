#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "j1Player.h"


class j1EntityManager : public j1Module
{
public:

	p2List<j1Entity*> entities;
	j1Player* player;
	j1EntityManager();
	~j1EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool PostUpdate();

	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	bool CreateEntity(EntityType type, fPoint position);
	j1Entity* getEntity(EntityType type);
	bool DeleteEntity(j1Entity* entity);
	bool OnCollision(Collider* c1, Collider* c2);

	bool draw_path = false;

private:
	pugi::xml_node entity_configs;
	int id_count = 0;
};

#endif
