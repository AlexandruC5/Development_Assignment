#ifndef __J1SCENEMOUNTAIN_H__
#define __J1SCENEMOUNTAIN_H__

#include "j1Scene.h"
class j1SceneMountain : public j1Scene
{
public:
	j1SceneMountain();

	// Destructor
	virtual ~j1SceneMountain();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	bool Update(float dt);
	bool OnCollision(Collider * c1, Collider * c2) override;
};

#endif