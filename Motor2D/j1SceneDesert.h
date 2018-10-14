#ifndef __j1SCENEDESERT_H__
#define __j1SCENEDESERT_H__

#include "j1Scene.h"
class j1SceneDesert : public j1Scene
{
public:
	j1SceneDesert();

	// Destructor
	virtual ~j1SceneDesert();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	bool Update(float dt);
	bool OnCollision(Collider * c1, Collider * c2) override;
};

#endif