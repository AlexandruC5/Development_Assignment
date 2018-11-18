#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "j1Animation.h"

#define TOTAL_ANIMATIONS 6

class j1Player : public j1Entity
{
private:
	bool charge = false;

	//charged jump variables
	float charge_value = 0.0F;
	float charged_time = 0.0F;
	float boost_x = 0.0F;
	float max_charge = 0.0F;
	float charge_increment = 0.0F;

	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void ChargingUpdate();
	void GodUpdate();
	void Jump(float boost_y);
	void Die();

public:
	j1Player(EntityType type, pugi::xml_node, fPoint position, p2SString id);
	~j1Player();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool CleanUp();

	void CheckDeath();
	void ResetPlayer();
	void OnCollision(Collider* c1, Collider* c2);
};

#endif