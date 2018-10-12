#ifndef __J1SCENEFOREST_H__
#define __J1SCENEFOREST_H__

#include "j1Scene.h"
class j1SceneForest : public j1Scene
{
public:
	j1SceneForest();

	// Destructor
	virtual ~j1SceneForest();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool Update(float dt) override;
};

#endif