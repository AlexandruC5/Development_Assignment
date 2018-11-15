#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1SwapScene.h"
#include "j1Scene.h"
#include "p2Defs.h"
#include "j1Audio.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1EntityManager.h"


j1Player::j1Player(EntityType type) : j1Entity(type)
{}

j1Player::~j1Player()
{}

bool j1Player::Awake(pugi::xml_node &conf)
{
	//load spritesheet
	sprite_route = PATH(conf.child("folder").child_value(), conf.child("sprite").child_value());

	//load animations
	int index = 0;
	pugi::xml_node animation;
	for (animation = conf.child("animations").first_child(); animation; animation = animation.next_sibling())
	{
		pugi::xml_node frame;
		for (frame = animation.child("frame"); frame; frame = frame.next_sibling("frame"))
		{
			animations[index].PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
		}
		animations[index].speed = animation.attribute("speed").as_float();
		animations[index].loop = animation.attribute("loops").as_bool(true);
		index++;
	}

	//load player stats
	movement_speed = conf.child("movement_speed").attribute("value").as_float();
	jump_speed = conf.child("jump_speed").attribute("value").as_float();
	acceleration = conf.child("acceleration").attribute("value").as_float();
	threshold = conf.child("threshold").attribute("value").as_float();
	gravity = conf.child("gravity").attribute("value").as_float();
	fall_speed = conf.child("fall_speed").attribute("value").as_float();
	charged_time = conf.child("charged_jump").attribute("time").as_float();
	charge_increment = conf.child("charged_jump").attribute("charge_increment").as_float();
	max_charge = conf.child("charged_jump").attribute("max_charge").as_float();

	animation_frame = { 0, 0, conf.child("collider").attribute("width").as_int(), conf.child("collider").attribute("height").as_int() };
	collider_offset = conf.child("collider").attribute("offset").as_int();
	jump_fx = App->audio->LoadFx(conf.child("audio").child("jump_fx").child_value());
	return true;
}

bool j1Player::Start()
{
	sprite = App->tex->Load(sprite_route.GetString());
	collider = App->collision->AddCollider(animation_frame, COLLIDER_PLAYER, App->entitymanager, true);
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
	case DEAD:
		animation_frame = animations[DEAD].GetCurrentFrame();
		break;
	case CHARGE: ChargingUpdate();
		break;
	case WIN: 
		target_speed.x = 0.0F;
		animation_frame = animations[WIN].GetCurrentFrame();
		break;
	case GOD: GodUpdate();
		break;
	default:
		break;
	}

	
	return true;
}

bool j1Player::Update(float dt)
{
	if (state == JUMPING)
	{
		target_speed.y += gravity * dt;
		if (target_speed.y > fall_speed) target_speed.y = fall_speed; //limit falling speed
		velocity.y = floor((target_speed.y * acceleration + velocity.y * (1 - acceleration))*dt);
	}
	else if(state == CHARGE)
	{
		if (charge_value < max_charge)
			charge_value += charge_increment*dt;
	}
	else if (charge)
	{
		if (charge_value < max_charge)
			charge_value += charge_increment * dt;
	}

	velocity.x = floor((target_speed.x * acceleration + velocity.x * (1 - acceleration))*dt);

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if(state != GOD) state = GOD;
		else state = IDLE;
	}

	StepY(dt);
	StepX(dt);

	App->render->Blit(sprite, position.x, position.y, &animation_frame, 1.0f, flipX);	
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
	if (!is_grounded) state = JUMPING;

	return true;
}

void j1Player::IdleUpdate() 
{
	target_speed.x = 0.0F;
	animation_frame = animations[IDLE].GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) != App->input->GetKey(SDL_SCANCODE_A)) state = MOVING;
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		charge = true;
		if (charge_value >= charged_time)
		{
			state = CHARGE;
			charge = false;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		target_speed.y = -jump_speed;
		is_grounded = false;
		state = JUMPING;
		charge_value = 0.0F;
		Jump(0.0F);
	}
		
	if (!is_grounded) state = JUMPING;
}

void j1Player::MovingUpdate() 
{
	animation_frame = animations[MOVING].GetCurrentFrame();
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
		charge = true;
		if (charge_value >= charged_time)
		{
			state = CHARGE;
			charge = false;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		target_speed.y = -jump_speed;
		is_grounded = false;
		state = JUMPING;
		charge_value = 0.0F;
		Jump(0.0F);
	}

	if (!is_grounded) state = JUMPING;
}

void j1Player::JumpingUpdate() 
{
	animation_frame = animations[JUMPING].GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A))
	{
		target_speed.x = 0.0F;
		boost_x = 0.0F;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (target_speed.x < 0.0F) boost_x = 0.0F;
		target_speed.x = movement_speed + boost_x;
		flipX = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (target_speed.x > 0.0F) boost_x = 0.0F;
		target_speed.x = -movement_speed - boost_x;
		flipX = false;
	}

	if (is_grounded) 
	{
		if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A)) state = IDLE;
		else state = MOVING;

		target_speed.y = 0.0F;
		velocity.y = 0.0F;
		boost_x = 0.0F;
	}
}

void j1Player::ChargingUpdate()
{
	target_speed.x = 0.0F;
	animation_frame = animations[CHARGE].GetCurrentFrame();
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{
			boost_x = charge_value;
			Jump(charge_value/2.0F); //since we jump diagonal the y jump force is halved
		}
		else Jump(charge_value);
	}
	else if (!is_grounded)
	{
		state = JUMPING;
		charge_value = 0.0F;
	}
}

void j1Player::Jump(float boost_y)
{
	target_speed.y = -jump_speed - boost_y;
	is_grounded = false;
	state = JUMPING;
	charge_value = 0;
	App->audio->PlayFx(jump_fx);
}

bool j1Player::Load(pugi::xml_node &player) 
{
	position.x = player.child("position").attribute("x").as_float();
	position.y = player.child("position").attribute("y").as_float();

	velocity.x = player.child("velocity").attribute("x").as_float();
	velocity.y = player.child("velocity").attribute("y").as_float();

	target_speed.x = player.child("target_speed").attribute("x").as_float();
	target_speed.y = player.child("target_speed").attribute("y").as_float();

	state = (EntityState)player.child("state").attribute("value").as_int();
	is_grounded = player.child("is_grounded").attribute("value").as_bool();
	flipX = player.child("flipX").attribute("value").as_bool();

	collider->SetPos(position.x, position.y + collider_offset); //set collider to loaded position

	return true;
}


bool j1Player::Save(pugi::xml_node &player) const
{
	//save position of player
	pugi::xml_node position_node = player.append_child("position");
	position_node.append_attribute("x") = position.x;
	position_node.append_attribute("y") = position.y;

	//save current speed of palyer
	pugi::xml_node velocity_node = player.append_child("velocity");
	velocity_node.append_attribute("x") = velocity.x;
	velocity_node.append_attribute("y") = velocity.y;

	pugi::xml_node target_speed_node = player.append_child("target_speed");
	target_speed_node.append_attribute("x") = target_speed.x;
	target_speed_node.append_attribute("y") = target_speed.y;

	//save state of player
	player.append_child("state").append_attribute("value") = state != DEAD ? (int)state : (int)IDLE;
	player.append_child("is_grounded").append_attribute("value") = is_grounded;
	player.append_child("flipX").append_attribute("value") = flipX;

	return true;
}

void j1Player::CheckDeath()
{
	if(position.y > App->map->data.height * App->map->data.tile_height && state != DEAD && state != GOD)
	{
		state = DEAD;
		velocity.x = 0.0F;
		target_speed.x = 0.0F;
		App->swap_scene->Reload();
	}
}

void j1Player::StepX(float dt)
{
	if (state != GOD)
	{
		if (velocity.x > 0) velocity.x = MIN(velocity.x, App->collision->DistanceToRightCollider(collider)); //movement of the player is min between distance to collider or his velocity
		else if (velocity.x < 0) velocity.x = MAX(velocity.x, App->collision->DistanceToLeftCollider(collider)); //movement of the player is max between distance to collider or his velocity
	}
	if (fabs(velocity.x) < threshold) velocity.x = 0.0F;
	position.x += velocity.x;
	collider->rect.x = position.x;
}

void j1Player::StepY(float dt)
{
	if (state != GOD) 
	{
		if (velocity.y < 0) 
		{
			velocity.y = MAX(velocity.y, App->collision->DistanceToTopCollider(collider)); //movement of the player is max between distance to collider or his velocity
			if (velocity.y == 0) target_speed.y = 0.0F;
		}
		else
		{
			float distance = App->collision->DistanceToBottomCollider(collider);
			velocity.y = MIN(velocity.y, distance); //movement of the player is min between distance to collider or his velocity
			is_grounded = (distance == 0) ? true : false;
		}
	}
	if (fabs(velocity.y) < threshold) velocity.y = 0.0F;
	position.y += velocity.y;
	collider->rect.y = position.y + collider_offset;
}

void j1Player::ResetPlayer()
{
	state = IDLE;
	velocity = { 0.0F, 0.0F };
	target_speed = { 0.0F, 0.0F };
	flipX = true;
}

void j1Player::SetPosition(float x, float y)
{
	position = { x,y };
	if(collider) collider->SetPos(position.x, position.y);
}

void j1Player::GodUpdate()
{
	animation_frame = animations[JUMPING].GetCurrentFrame();
	if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A)) target_speed.x = 0.0F;
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
	if (App->input->GetKey(SDL_SCANCODE_W) == App->input->GetKey(SDL_SCANCODE_S)) target_speed.y = 0.0F;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) target_speed.y = -movement_speed;
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) target_speed.y = movement_speed;
}