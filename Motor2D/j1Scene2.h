#ifndef __J1SCENE2_H__
#define __J1SCENE2_H__

#include "j1Scene.h"
class j1Scene2 : public j1Scene
{
public:
	j1Scene2();

	// Destructor
	virtual ~j1Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);
};

#endif