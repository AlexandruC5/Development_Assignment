#pragma once
#include "j1Enemy.h"

#define TOTAL_ANIMATIONS 1
#define MINIMUM_DISTANCE 700

class j1FlyingEnemy : public j1Enemy
{
protected:
	bool moving_down = false;
	void JumpingUpdate();
	void ResetPathfindingVariables();
	void PathfindY();

public:
	j1FlyingEnemy(EntityType type, pugi::xml_node, fPoint position, p2SString id);
	~j1FlyingEnemy();

	bool PreUpdate();
	bool Update(float dt);
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
};

