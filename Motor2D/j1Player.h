#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Animation.h"


enum Player_State {
	NO_STATE,
	IDLE,
	MOVING,
	JUMPING,
	DEAD
};
class j1Player : public j1Module
{
private:
	pugi::xml_node player_config;
	SDL_Texture* sprite;
	Collider* coll;

	float movement_speed = 0.0F;
	float jump_speed = 0.0F;
	float gravity = 0.0F;
	float acceleration = 0.0F;
	float threshold = 0.0F;
	bool flipX = false;
	bool isGrounded = false;
	fPoint target_speed = { 0.0F, 0.0F };

	Animation idle;
	Animation move;
	Animation jump;
	Animation current_animation;
public:
	fPoint position = { 0.0F, 0.0F };
	fPoint velocity = { 0.0F, 0.0F };
	Player_State state = IDLE;

	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PostUpdate();
	bool Update(float dt);
	bool CleanUp();
	bool OnCollision(Collider* c1, Collider* c2);

	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
};

#endif