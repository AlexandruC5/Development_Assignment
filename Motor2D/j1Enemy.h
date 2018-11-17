#include "j1Entity.h"
#include "p2Point.h"
#include "j1Animation.h"
#include "p2DynArray.h"

#define TOTAL_ANIMATIONS 1
#define MINIMUM_DISTANCE 700

class j1Enemy : public j1Entity
{
protected:
	void StepX(float dt);
	void StepY(float dt);
	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void Jump();

	//Pathfinding
	int current_destination = 0;
	int previous_destination = 0;
	int next_destination = -1;
	int jump_height = 2;
	bool reached_X = false;
	bool reached_Y = false;
	bool current_is_grounded = false;

	bool moving_right = false;
	bool moving_left = false;
	bool jump = false;
	bool chase = false;

	bool GetPath();

public:
	p2DynArray<iPoint> current_path;
	j1Enemy(EntityType type, pugi::xml_node);
	~j1Enemy();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
};

