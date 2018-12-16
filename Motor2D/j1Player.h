#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "j1Animation.h"

class j1Player : public j1Entity
{
private:
	//charged jump variables
	bool charge = false;
	float charge_value = 0.0F;
	float charged_time = 0.0F;
	float boost_x = 0.0F;
	float max_charge = 0.0F;
	float charge_increment = 0.0F;
	int starting_lives = 0;

	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void ChargingUpdate();
	void GodUpdate();
	void Jump(float boost_y);
	void Die();

public:
	j1Player(EntityType type, pugi::xml_node, fPoint position, p2SString id,int clone_number);
	~j1Player();

	int clone_number = 0;
	int score;
	int lives = 3;
	bool Update(float dt);
	bool PreUpdate();

	void CheckDeath();
	void OnCollision(Collider* c1, Collider* c2);
	void ResetLives();
	void ResetScale();
};

#endif