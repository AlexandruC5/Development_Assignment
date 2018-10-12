#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1SwapScene.h"
#include "j1Scene.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Player.h"


j1Player::j1Player()
{
	name.create("player");
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node &conf)
{
	//load spritesheet
	sprite_route = conf.child("folder").child_value();
	sprite_route += conf.child("sprite").child_value();

	//load animations
	pugi::xml_node frame;
	for (frame = conf.child("animations").child("idle").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		idle.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	idle.speed = conf.child("animations").child("idle").attribute("speed").as_float();

	for (frame = conf.child("animations").child("move").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		move.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	move.speed = conf.child("animations").child("move").attribute("speed").as_float();

	for (frame = conf.child("animations").child("jump").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		jump.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	jump.speed = conf.child("animations").child("jump").attribute("speed").as_float();
	jump.loop = false;

	for (frame = conf.child("animations").child("die").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		die.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	die.speed = conf.child("animations").child("die").attribute("speed").as_float();

	for (frame = conf.child("animations").child("charge").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		charge.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	charge.speed = conf.child("animations").child("charge").attribute("speed").as_float();


	//load player stats
	movement_speed = conf.child("movement_speed").attribute("value").as_float();
	jump_speed = conf.child("jump_speed").attribute("value").as_float();
	acceleration = conf.child("acceleration").attribute("value").as_float();
	threshold = conf.child("threshold").attribute("value").as_float();
	gravity = conf.child("gravity").attribute("value").as_float();

	animation_frame = { 0, 0, conf.child("collider").attribute("width").as_int(), conf.child("collider").attribute("height").as_int() };
	collider_offset = conf.child("collider").attribute("offset").as_int();
	return true;
}

bool j1Player::Start()
{
	sprite = App->tex->Load(sprite_route.GetString());
	collider = App->collision->AddCollider(animation_frame, COLLIDER_PLAYER, this);
	collider->rect.x = position.x;
	collider->rect.y = position.y + collider_offset;
	return true;
}

bool j1Player::PreUpdate() 
{
	switch (state) {
	case IDLE: IdleUpdate();
		break;
	case MOVING: MovingUpdate();
		break;
	case JUMPING: JumpingUpdate();
		break;
	case DEAD: animation_frame = die.GetCurrentFrame();
		break;
	case CHARGE: ChargingUpdate();
		break;
	default:
		break;
	}

	velocity = target_speed * acceleration + velocity * (1 - acceleration);

	return true;
}

bool j1Player::Update(float dt)
{
	StepY();
	StepX();

	CheckDeath();

	App->render->Blit(sprite, position.x, position.y, &animation_frame, 1.0F, flipX);	
	return true;
}


bool j1Player::CleanUp()
{
	if (sprite) 
	{
		App->tex->UnLoad(sprite);
		sprite = nullptr;
	}

	if (collider) 
	{
		collider->to_delete = true;
		collider = nullptr;
	}
	if (!isGrounded) state = JUMPING;

	return true;
}

void j1Player::IdleUpdate() 
{
	target_speed.x = 0.0F;
	animation_frame = idle.GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) != App->input->GetKey(SDL_SCANCODE_A)) state = MOVING;
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		timer+=0.5;
		if (timer >= 10)
		{
			state = CHARGE;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		Jump(0);
	}
		
	if (!isGrounded) state = JUMPING;


}

void j1Player::MovingUpdate() 
{
	animation_frame = move.GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A))
	{
		state = IDLE;
		target_speed.x = 0.0F;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)	
	{
		target_speed.x = movement_speed;
		flipX = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		target_speed.x = -movement_speed;
		flipX = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		timer+=0.5;
		if (timer >= 10)
		{
			state = CHARGE;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		Jump(0);
	}

	

	if (!isGrounded) state = JUMPING;
}

void j1Player::JumpingUpdate() 
{
	target_speed.y += gravity;
	animation_frame = jump.GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A))
	{
		target_speed.x = 0.0F;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		target_speed.x = movement_speed;
		flipX = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		target_speed.x = -movement_speed;
		flipX = false;
	}

	if (isGrounded) 
	{
		if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A)) state = IDLE;
		else state = MOVING;

		target_speed.y = 0.0F;
		velocity.y = 0.0F;
	}
}

void j1Player::ChargingUpdate()
{
	target_speed.x = 0.0F;
	animation_frame = charge.GetCurrentFrame();
	if (timer < 30)
		timer+=0.5;
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		Jump(timer);
	}
}

void j1Player::Jump(float boost)
{
	target_speed.y = -jump_speed + boost;
	isGrounded = false;
	state = JUMPING;
	timer = 0;
}

bool j1Player::Load(pugi::xml_node &player) 
{
	position.x = player.child("position").attribute("x").as_float();
	position.y = player.child("position").attribute("y").as_float();
	state = (Player_State)player.child("state").attribute("value").as_int();
	isGrounded = player.child("is_grounded").attribute("value").as_bool();
	velocity.x = player.child("velocity").attribute("x").as_float();
	velocity.y = player.child("velocity").attribute("y").as_float();
	target_speed.x = player.child("target_speed").attribute("x").as_float();
	target_speed.y = player.child("target_speed").attribute("y").as_float();

	collider->SetPos(position.x, position.y + collider_offset);

	return true;
}


bool j1Player::Save(pugi::xml_node &player) const
{
	pugi::xml_node position_node = player.append_child("position");
	position_node.append_attribute("x") = position.x;
	position_node.append_attribute("y") = position.y;
	player.append_child("state").append_attribute("value") = (int)state;
	player.append_child("is_grounded").append_attribute("value") = isGrounded;
	pugi::xml_node velocity_node = player.append_child("velocity");
	velocity_node.append_attribute("x") = velocity.x;
	velocity_node.append_attribute("y") = velocity.y;
	pugi::xml_node target_speed_node = player.append_child("target_speed");
	target_speed_node.append_attribute("x") = target_speed.x;
	target_speed_node.append_attribute("y") = target_speed.y;

	return true;
}

void j1Player::CheckDeath()
{
	if(position.y > App->map->data.height * App->map->data.tile_height && state != DEAD)
	{
		state = DEAD;
		velocity.x = 0.0F;
		App->swap_scene->Reload();
	}
}

void j1Player::StepX()
{
	if (velocity.x > 0) velocity.x = MIN(velocity.x, App->collision->DistanceToRightCollider(collider));
	else velocity.x = MAX(velocity.x, App->collision->DistanceToLeftCollider(collider));
	if (fabs(velocity.x) < threshold) velocity.x = 0.0F;

	position.x += velocity.x;
	collider->rect.x = position.x;
}

void j1Player::StepY()
{
	if (velocity.y < 0) velocity.y = MAX(velocity.y, App->collision->DistanceToTopCollider(collider));
	else
	{
		float distance = App->collision->DistanceToBottomCollider(collider);
		velocity.y = MIN(velocity.y, distance);
		isGrounded = (distance == 0) ? true : false;
	}
	if (fabs(velocity.y) < threshold) velocity.y = 0.0F;

	position.y += velocity.y;
	collider->rect.y = position.y + collider_offset;
}

void j1Player::ResetPlayer()
{
	state = IDLE;
	velocity = { 0.0F,0.0F };
	target_speed = { 0.0F,0.0F };
}