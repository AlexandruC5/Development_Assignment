#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Enemy.h"


j1Enemy::j1Enemy()
{
}


j1Enemy::~j1Enemy()
{
}

bool j1Enemy::Awake(pugi::xml_node &)
{
	animations[E_IDLE].PushBack({14,1,37,33});
	animations[E_IDLE].speed = 1;
	animations[E_IDLE].loop = true;

	return true;
}

bool j1Enemy::Start()
{
	sprite = App->tex->Load("textures/dead_buny_floor_spritesheet.png");

	movement_speed = 600.0F;
	jump_speed = 1250.0F;
	acceleration = 0.8F;
	threshold = 0.4F;
	gravity = 1600.0f;
	fall_speed = 1250.0F;


	position = { 120.0F, 2091.0F };

	animation_frame = animations[E_IDLE].GetCurrentFrame();
	collider = App->collision->AddCollider(animation_frame, COLLIDER_PLAYER, this, true);
	collider->rect.x = position.x;
	collider->rect.y = position.y + collider_offset;
	
	return true;
}

bool j1Enemy::Update(float dt)
{
	if (state == E_JUMPING)
	{
		target_speed.y += gravity*dt;
		if (target_speed.y > fall_speed) target_speed.y = fall_speed; //limit falling speed
		velocity.y = (target_speed.y * acceleration + velocity.y * (1 - acceleration))*dt;
	}
	velocity.x = (target_speed.x * acceleration + velocity.x * (1 - acceleration))*dt;

	StepY(dt);
	StepX(dt);
	CheckDeath();

	animation_frame = animations[E_IDLE].GetCurrentFrame();
	App->render->Blit(sprite, position.x, position.y, &animation_frame, 1.0f, flipX);

	return true;
}

bool j1Enemy::PreUpdate()
{
	if (current_path.Count() > 0)
	{
		moving_right = false;
		moving_left = false;
		jump = false;

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
		}

		if (reached_X && reached_Y)
		{
			LOG("reached node %i with position x %i y %i", current_destination, current_path.At(current_destination)->x, current_path.At(current_destination)->y);
			previous_destination = current_destination;
			current_destination++;
			next_destination = current_destination + 1;
			if (next_destination >= current_path.Count())
				next_destination = -1;

			if (current_destination >= current_path.Count())
			{
				current_path.Clear();
				current_destination = 1;
				previous_destination = 0;
				next_destination = 2;
			}
		}
	}

	switch (state) {
	case E_IDLE: IdleUpdate();
		break;
	case E_MOVING: MovingUpdate();
		break;
	case E_JUMPING: JumpingUpdate();
		break;
	case E_DEAD:
		animation_frame = animations[E_DEAD].GetCurrentFrame();
		break;
	default:
		break;
	}

	return true;
}

bool j1Enemy::CleanUp()
{
	return true;
}

bool j1Enemy::Load(pugi::xml_node &)
{
	return true;
}

bool j1Enemy::Save(pugi::xml_node &) const
{
	return true;
}

void j1Enemy::CheckDeath()
{

}

void j1Enemy::ResetPlayer()
{

}

void j1Enemy::SetPosition(float x, float y)
{

}

void j1Enemy::StepX(float dt)
{
	if (velocity.x > 0) 
		velocity.x = MIN(velocity.x, App->collision->DistanceToRightCollider(collider)); //movement of the player is min between distance to collider or his velocity
	else if (velocity.x < 0)
		velocity.x = MAX(velocity.x, App->collision->DistanceToLeftCollider(collider)); //movement of the player is max between distance to collider or his velocity

	if (fabs(velocity.x) < threshold) 
		velocity.x = 0.0F;

	position.x += velocity.x;
	collider->rect.x = position.x;
}

void j1Enemy::StepY(float dt)
{
	if (velocity.y < 0)
	{
		velocity.y = MAX(velocity.y, App->collision->DistanceToTopCollider(collider)); //movement of the player is max between distance to collider or his velocity
		if (velocity.y == 0) 
			target_speed.y = 0.0F;
	}
	else
	{
		float distance = App->collision->DistanceToBottomCollider(collider);
		velocity.y = MIN(velocity.y, distance); //movement of the player is min between distance to collider or his velocity
		is_grounded = (distance == 0) ? true : false;
	}

	if (fabs(velocity.y) < threshold)
		velocity.y = 0.0F;

	position.y += velocity.y;
	collider->rect.y = position.y + collider_offset;
}

void j1Enemy::IdleUpdate()
{
	target_speed.x = 0.0F;
	if (moving_left != moving_right) 
		state = E_MOVING;
	if (jump) Jump();

	if (!is_grounded) state = E_JUMPING;
}

void j1Enemy::MovingUpdate()
{
	if (moving_left == moving_right)
	{
		state = E_IDLE;
		target_speed.x = 0.0F;
	}
	else if (moving_right)
	{
		target_speed.x = movement_speed;
		flipX = true;
	}
	else if (moving_left)
	{
		target_speed.x = -movement_speed;
		flipX = false;
	}

	if (jump)
	{
		Jump();
	}

	if (!is_grounded) 
		state = E_JUMPING;
}

void j1Enemy::JumpingUpdate()
{
	if (moving_left == moving_right)
	{
		target_speed.x = 0.0F;
	}
	else if (moving_right)
	{
		target_speed.x = movement_speed;
		flipX = true;
	}
	else if (moving_left)
	{
		target_speed.x = -movement_speed;
		flipX = false;
	}

	if (is_grounded)
	{
		if (moving_left == moving_right) state = E_IDLE;
		else state = E_MOVING;

		target_speed.y = 0.0F;
		velocity.y = 0.0F;
	}
}

void j1Enemy::Jump()
{
	target_speed.y = -jump_speed;
	is_grounded = false;
	state = E_JUMPING;
}