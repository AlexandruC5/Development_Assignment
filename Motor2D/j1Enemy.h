#include "j1Module.h"
#include "p2Point.h"
#include "j1Animation.h"
#include "p2DynArray.h"

enum Enemy_State {
	E_NO_STATE = -1,
	E_IDLE,
	E_MOVING,
	E_JUMPING,
	E_DEAD,
	E_CHARGE,
	E_WIN,
	E_TOTAL_ANIMATIONS,
	E_GOD,
};

class j1Enemy : public j1Module
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

	int collider_offset = 0;
	bool flipX = true;
	bool is_grounded = true;


	Animation animations[E_TOTAL_ANIMATIONS];
	SDL_Rect animation_frame;

	unsigned int jump_fx;

	void StepX(float dt);
	void StepY(float dt);
	void IdleUpdate();
	void MovingUpdate();
	void JumpingUpdate();
	void Jump();

	bool moving_right = false;
	bool moving_left = false;
	bool jump = false;


	//pathfinding variables

	int current_destination = 0;
	int previous_destination = 0;
	int next_destination = 1;
	bool reached_X = false;
	bool reached_Y = false;
	bool current_is_grounded = false;

public:
	p2DynArray<iPoint> current_path;
	j1Enemy();
	~j1Enemy();

	Collider* collider;
	fPoint position = { 0.0F, 0.0F };
	fPoint velocity = { 0.0F, 0.0F };
	Enemy_State state = E_IDLE;
	float threshold = 0.0F;

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PreUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void CheckDeath();
	void ResetPlayer();
	void SetPosition(float x, float y);
};

