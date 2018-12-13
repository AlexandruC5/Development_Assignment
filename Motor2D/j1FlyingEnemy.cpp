#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1EntityManager.h"
#include "j1Input.h"
#include "j1FlyingEnemy.h"


j1FlyingEnemy::j1FlyingEnemy(EntityType type, pugi::xml_node config, fPoint position, p2SString id):j1Enemy(type, config, position, id)
{
	state = MOVING;
	jump_height = -1;
	ignore_platforms = true;
}


j1FlyingEnemy::~j1FlyingEnemy()
{
}

bool j1FlyingEnemy::PreUpdate()
{
	//Check start chase
	if (position.DistanceManhattan(App->entitymanager->player->position) < chase_distance)
		chase = true;
	else
		chase = false;

	PathfindingPreupdate();

	switch (state) {
	case MOVING: MovingUpdate();
		break;
	case DEAD:
		//TODO Die animation
		break;
	default:
		break;
	}

	return true;
}

bool j1FlyingEnemy::Update(float dt)
{
	PathfindingUpdate();

	velocity = (target_speed * acceleration + velocity * (1 - acceleration))*dt;
	StepY();
	StepX();

	animation_frame = animations[IDLE].GetCurrentFrame(dt);
	return true;
}

bool j1FlyingEnemy::Load(pugi::xml_node &conf)
{
	j1Enemy::Load(conf);
	moving_down = conf.child("movement_controls").attribute("moving_down").as_bool();
	return true;
}

bool j1FlyingEnemy::Save(pugi::xml_node &conf) const
{
	j1Enemy::Save(conf);
	conf.append_child("movement_controls").append_attribute("moving_down") = moving_down;
	return true;
}

void j1FlyingEnemy::MovingUpdate()
{
	if (moving_left == moving_right)
	{
		target_speed.x = 0.0F;
	}
	else if (moving_right)
	{
		target_speed.x = movement_speed;
		flipX = false;
	}
	else if (moving_left)
	{
		target_speed.x = -movement_speed;
		flipX = true;
	}

	if (jump == moving_down)
	{
		target_speed.y = 0.0F;
	}
	else if (jump)
	{
		target_speed.y = -movement_speed;
	}
	else if (moving_down)
	{
		target_speed.y = movement_speed;
	}

}

void j1FlyingEnemy::ResetPathfindingVariables()
{
	moving_right = false;
	moving_left = false;
	jump = false;
	moving_down = false;
}

void j1FlyingEnemy::PathfindY()
{
	reached_Y = (current_path.At(previous_destination)->y <= current_path.At(current_destination)->y && pivot.y >= current_path.At(current_destination)->y)
		|| (current_path.At(previous_destination)->y >= current_path.At(current_destination)->y && pivot.y <= current_path.At(current_destination)->y);

	if (abs(pivot.y - current_path.At(current_destination)->y) > POSITION_ERROR_Y)
		reached_Y = false;

	if (!reached_Y)
	{
		if (pivot.y > current_path.At(current_destination)->y)
			jump = true;
		else if (pivot.y < current_path.At(current_destination)->y)
			moving_down = true;
	}
}

