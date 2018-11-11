#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Animation.h"


enum Player_State {
	NO_STATE = -1,
	IDLE,
	MOVING,
	JUMPING,
	DEAD,
	CHARGE,
	WIN,
	TOTAL_ANIMATIONS,
	GOD,
};
class j1Player : public j1Module
{
private:
	p2SString sprite_route;
	SDL_Texture* sprite;

	//movement variables
	float movement_speed = 0.0F;
	float jump_speed = 0.0F;
	float gravity = 0.0F;
	float acceleration = 0.0F;
	float fall_speed = 0.0F;
	fPoint target_speed = { 0.0F, 0.0F };
	float dt_;
	
	int collider_offset = 0;
	bool flipX = true;
	bool is_grounded = true;

	//charged jump variables
	float charge_value = 0.0F;
	float charged_time = 0.0F;
	float boost_x = 0.0F;
	float max_charge = 0.0F;
	float charge_increment = 0.0F;

	Animation animations[TOTAL_ANIMATIONS];
	SDL_Rect animation_frame;

	unsigned int jump_fx;

	void StepX();
	void StepY();
	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void ChargingUpdate();
	void GodUpdate();
	void Jump(const float &boost_y);

public:
	Collider* collider;
	fPoint position = { 0.0F, 0.0F };
	fPoint velocity = { 0.0F, 0.0F };
	Player_State state = IDLE;
	float threshold = 0.0F;

	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void CheckDeath();
	void ResetPlayer();
	void SetPosition(const float &x, const float &y);
};

#endif