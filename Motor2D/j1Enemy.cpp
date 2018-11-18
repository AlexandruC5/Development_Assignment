#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Enemy.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"


j1Enemy::j1Enemy(EntityType type, pugi::xml_node config, fPoint position, p2SString id) : j1Entity(type, config, position, id)
{
	chase_distance = config.child("chase_distance").attribute("value").as_float();

	collider = App->collision->AddCollider(animation_frame, COLLIDER_ENEMY, App->entitymanager, false);
	collider->rect.x = position.x;
	collider->rect.y = position.y + collider_offset;
}


j1Enemy::~j1Enemy()
{}


bool j1Enemy::Update(float dt)
{
	BROFILER_CATEGORY("Update_Enemy", Profiler::Color::LightSalmon);
	PathfindingUpdate();

	if (state == JUMPING)
	{
		target_speed.y += gravity*dt;
		if (target_speed.y > fall_speed) target_speed.y = fall_speed; //limit falling speed
	}

	velocity = (target_speed * acceleration + velocity * (1 - acceleration))*dt;
	StepY();
	StepX();

	animation_frame = animations[state].GetCurrentFrame(dt);
	App->render->Blit(sprite, position.x, position.y, &animation_frame, 1.0f, flipX);

	return true;
}

bool j1Enemy::PreUpdate()
{
	if (position.DistanceManhattan(App->entitymanager->player->position) < chase_distance) 
		chase = true;
	else 
		chase = false;

	PathfindingPreupdate();

	switch (state) {
	case IDLE: IdleUpdate();
		break;
	case MOVING: MovingUpdate();
		break;
	case JUMPING: JumpingUpdate();
		break;
	case DEAD:
		break;
	default:
		break;
	}

	return true;
}

bool j1Enemy::Load(pugi::xml_node &conf)
{
	j1Entity::Load(conf);
	state = (EntityState) conf.child("state").attribute("value").as_int();
	chase = conf.child("chase").attribute("value").as_bool();
	moving_right = conf.child("movement_controls").attribute("moving_right").as_bool();
	moving_left = conf.child("movement_controls").attribute("moving_left").as_bool();
	jump = conf.child("movement_controls").attribute("jump").as_bool();

	current_path.Clear();
	return true;
}

bool j1Enemy::Save(pugi::xml_node &conf) const
{
	j1Entity::Save(conf);
	conf.append_child("state").append_attribute("value") = state;
	conf.append_child("chase").append_attribute("value") = chase;
	conf.append_child("movement_controls").append_attribute("moving_right") = moving_right;
	conf.append_child("movement_controls").append_attribute("moving_left") = moving_left;
	conf.append_child("movement_controls").append_attribute("jump") = jump;
	return true;
}

void j1Enemy::IdleUpdate()
{
	target_speed.x = 0.0F;
	if (moving_left != moving_right) 
		state = MOVING;
	if (jump) Jump();

	if (!is_grounded) state = JUMPING;
}

void j1Enemy::MovingUpdate()
{
	if (moving_left == moving_right)
	{
		state = IDLE;
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

	if (jump)
	{
		Jump();
	}

	if (!is_grounded) 
		state = JUMPING;
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
		flipX = false;
	}
	else if (moving_left)
	{
		target_speed.x = -movement_speed;
		flipX = true;
	}

	if (is_grounded)
	{
		if (moving_left == moving_right) state = IDLE;
		else state = MOVING;

		target_speed.y = 0.0F;
		velocity.y = 0.0F;
		total_jumps = 0;
	}

	if (jump && total_jumps < MAX_JUMPS)
	{
		Jump();
	}
}

void j1Enemy::Jump()
{
	target_speed.y = -jump_speed;
	is_grounded = false;
	state = JUMPING;
	total_jumps++;
}

bool j1Enemy::GetPath()
{
	iPoint new_destination = App->map->WorldToMap(App->entitymanager->player->pivot.x, App->entitymanager->player->pivot.y);

	if (new_destination != destination || current_path.Count() == 0)
	{
		iPoint origin = App->map->WorldToMap(pivot.x, pivot.y);
		destination = new_destination;

		App->pathfinding->CreatePath(origin, destination, jump_height);

		const p2DynArray<iPoint>* tmp_array = App->pathfinding->GetLastPath();
		current_path.Clear();
		for (int i = 0; i < tmp_array->Count(); i++)
		{
			iPoint p = App->map->MapToWorld(tmp_array->At(i)->x, tmp_array->At(i)->y);
			p.x += App->map->data.tile_width / 2;
			p.y += App->map->data.tile_height / 2 + App->entitymanager->player->collider_offset;
			current_path.PushBack(p);
		}
		current_destination = current_path.Count() > 1 ? 1 : 0;
		previous_destination = 0;
		next_destination = current_path.Count() > 2 ? 2 : -1;

		ResetPathfindingVariables();
	}
	

	return true;
}

void j1Enemy::PathfindingUpdate()
{
	if (chase && state != JUMPING)
		GetPath();
	if (App->entitymanager->draw_path)
		DrawPath();
}

void j1Enemy::PathfindingPreupdate()
{
	if (current_path.Count() > 0)
	{
		ResetPathfindingVariables();
		
		PathfindX();
		PathfindY();

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
}

void j1Enemy::ResetPathfindingVariables()
{
	moving_right = false;
	moving_left = false;
	jump = false;
}

void j1Enemy::PathfindX()
{
	reached_X = (current_path.At(previous_destination)->x <= current_path.At(current_destination)->x && current_path.At(current_destination)->x <= pivot.x)
		|| (current_path.At(previous_destination)->x >= current_path.At(current_destination)->x && current_path.At(current_destination)->x >= pivot.x);

	if (!reached_X)
	{
		if (pivot.x < current_path.At(current_destination)->x)
			moving_right = true;
	
		else if (pivot.x > current_path.At(current_destination)->x)
			moving_left = true;
	}
	else
	{
		if (next_destination > 0)
		{
			iPoint point = App->map->WorldToMap(current_path.At(next_destination)->x, current_path.At(next_destination)->y);
			if (!App->pathfinding->IsGround({ point.x, point.y+1 }))
			{
				moving_right = false;
				moving_left = false;
			}
			else
			{
				if (pivot.x < current_path.At(next_destination)->x)
					moving_right = true;
				else if (pivot.x > current_path.At(next_destination)->x)
					moving_left = true;
			}
		}
	}
}

void j1Enemy::PathfindY()
{
	reached_Y = (current_path.At(previous_destination)->y <= current_path.At(current_destination)->y && pivot.y >= current_path.At(current_destination)->y)
		|| (current_path.At(previous_destination)->y >= current_path.At(current_destination)->y && pivot.y <= current_path.At(current_destination)->y);

	if (!reached_Y)
	{
		if (pivot.y > current_path.At(current_destination)->y)
			jump = true;
	}
}

void j1Enemy::DrawPath()
{
	for (int i = 0; i < current_path.Count(); i++)
	{
		iPoint p = { current_path.At(i)->x, current_path.At(i)->y };
		p.x -= App->map->data.tile_width / 2;
		p.y -= App->map->data.tile_height / 2;

		SDL_Rect quad = { p.x, p.y, App->map->data.tile_width , App->map->data.tile_height };
		App->render->DrawQuad(quad, 255, 255, 0, 75, true);
	}
}