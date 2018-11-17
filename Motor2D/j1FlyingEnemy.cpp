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
	animations = new Animation[TOTAL_ANIMATIONS];
	LoadAnimations(config);
	state = JUMPING;

	jump_height = -1;
}


j1FlyingEnemy::~j1FlyingEnemy()
{
}

bool j1FlyingEnemy::Start()
{
	return true;
}

bool j1FlyingEnemy::PreUpdate()
{
	if (position.DistanceManhattan(App->entitymanager->player->position) < MINIMUM_DISTANCE)
		chase = true;
	else
		chase = false;

	if (current_path.Count() > 0)
	{
		moving_right = false;
		moving_left = false;
		jump = false;
		moving_down = false;

		reached_X = (current_path.At(previous_destination)->x <= current_path.At(current_destination)->x  && current_path.At(current_destination)->x <= position.x)
			|| (current_path.At(previous_destination)->x >= current_path.At(current_destination)->x && current_path.At(current_destination)->x >= position.x);

		reached_Y = (current_path.At(previous_destination)->y <= current_path.At(current_destination)->y && position.y >= current_path.At(current_destination)->y)
			|| (current_path.At(previous_destination)->y >= current_path.At(current_destination)->y && position.y <= current_path.At(current_destination)->y);


		if (!reached_X)
		{
			if (position.x < current_path.At(current_destination)->x)
				moving_right = true;
			else if (position.x > current_path.At(current_destination)->x)
				moving_left = true;
		}
		else
		{
			if (next_destination != -1 && position.x < current_path.At(next_destination)->x && is_grounded)
				moving_right = true;
			else if (next_destination != -1 && position.x > current_path.At(next_destination)->x && is_grounded)
				moving_left = true;
		}
		if (!reached_Y)
		{
			if (position.y > current_path.At(current_destination)->y)
				jump = true;
			else if (position.y < current_path.At(current_destination)->y)
				moving_down = true;
		}


		if (reached_X && reached_Y)
		{
			previous_destination = current_destination;
			current_destination++;
			next_destination = current_destination + 1;

			if (next_destination >= current_path.Count())
				next_destination = -1;

			if (current_destination >= current_path.Count())
				current_path.Clear();
		}
	}

	switch (state) {
	case JUMPING: JumpingUpdate();
		break;
	case DEAD:
		animation_frame = animations[DEAD].GetCurrentFrame();
		break;
	default:
		break;
	}

	return true;
}

bool j1FlyingEnemy::Update(float dt)
{
	if (chase)
	{
		GetPath();
		if (App->entitymanager->draw_path) DrawPath();
	}
	else
	{
		current_path.Clear();
		moving_right = false;
		moving_left = false;
		jump = false;
	}

	velocity = (target_speed * acceleration + velocity * (1 - acceleration))*dt;
	StepY(dt);
	StepX(dt);

	animation_frame = animations[IDLE].GetCurrentFrame();
	App->render->Blit(sprite, position.x, position.y, &animation_frame, 1.0f, flipX);
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

void j1FlyingEnemy::StepX(float dt)
{
	if (fabs(velocity.x) < threshold)
		velocity.x = 0.0F;

	position.y += velocity.y;
	collider->rect.y = position.y + collider_offset;
}

void j1FlyingEnemy::StepY(float dt)
{
	if (fabs(velocity.y) < threshold)
		velocity.y = 0.0F;

	position.x += velocity.x;
	collider->rect.x = position.x;
}

void j1FlyingEnemy::JumpingUpdate()
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

