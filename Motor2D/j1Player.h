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
	DEAD,
	CHARGE,
};
class j1Player : public j1Module
{
private:
	p2SString sprite_route;
	SDL_Texture* sprite;

	float movement_speed = 0.0F;
	float jump_speed = 0.0F;
	float gravity = 0.0F;
	float acceleration = 0.0F;
	float threshold = 0.0F;
	int collider_offset = 0;
	bool flipX = false;
	bool isGrounded = false;
	float timer = 0.0f;
	float charged_time = 0.0f;
	fPoint target_speed = { 0.0F, 0.0F };

	Animation idle;
	Animation move;
	Animation jump;
	Animation die;
	Animation charge;
	SDL_Rect animation_frame;

	void StepX();
	void StepY();
public:
	Collider* collider;
	fPoint new_position = { 0.0F, 0.0F };
	fPoint position = { 0.0F, 0.0F };
	fPoint velocity = { 0.0F, 0.0F };
	Player_State state = IDLE;

	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void ChargingUpdate();
	void Jump(float boost);

	void CheckDeath();
	void ResetPlayer();
};

#endif