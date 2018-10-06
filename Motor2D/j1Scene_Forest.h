#pragma once
#include "j1Scene.h"
class j1Scene_Forest : public j1Scene
{
public:
	j1Scene_Forest();

	// Destructor
	virtual ~j1Scene_Forest();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called before quitting
	bool CleanUp();
};

