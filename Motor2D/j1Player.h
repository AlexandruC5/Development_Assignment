#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Animation.h"


enum Player_State {
	NO_STATE,
	IDLE,
	MOVING,
	JUMP,
	DEAD
};
class j1Player : public j1Module
{
private:
	pugi::xml_node player_config;
	SDL_Texture* sprite;
	Collider* coll;
	float max_speed = 0.0f;
	float acceleration = 0.0f;
	fPoint target_speed = { 0.0f, 0.0f };

	Animation idle;
public:
	fPoint position = { 0.0f, 0.0f };
	fPoint velocity = { 0.0f, 0.0f };
	Player_State state = IDLE;

	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool CleanUp();
};

#endif