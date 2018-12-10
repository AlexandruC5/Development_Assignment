#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

struct Level {
	p2SString map_path = "";
	p2SString sound_path = "";
};

enum Scene{
	FOREST,
	DESERT,
	NONE,
};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node &node);
	bool Save(pugi::xml_node &node) const;

	p2List<Level> levels;

	Scene current_scene = FOREST;
};

#endif // __j1SCENE_H__