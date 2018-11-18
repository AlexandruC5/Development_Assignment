#include "j1Entity.h"
#include "p2Point.h"
#include "j1Animation.h"
#include "p2DynArray.h"

#define POSITION_ERROR 3.0F

class j1Enemy : public j1Entity
{
protected:
	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void Jump();
	void DrawPath();

	//Pathfinding
	float chase_distance = 0.0F;
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
	void PathfindingUpdate();
	void PathfindingPreupdate();
	virtual void ResetPathfindingVariables();
	virtual void PathfindX();
	virtual void PathfindY();

public:
	p2DynArray<iPoint> current_path;
	j1Enemy(EntityType type, pugi::xml_node, fPoint position, p2SString id);
	~j1Enemy();

	bool Update(float dt);
	bool PreUpdate();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
};

