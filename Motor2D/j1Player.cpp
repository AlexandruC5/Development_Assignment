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


	//load player stats
	movement_speed = conf.child("movement_speed").attribute("value").as_float();
	jump_speed = conf.child("jump_speed").attribute("value").as_float();
	acceleration = conf.child("acceleration").attribute("value").as_float();
	threshold = conf.child("threshold").attribute("value").as_float();
	gravity = conf.child("gravity").attribute("value").as_float();

	//player setup
	current_animation = idle;


	return true;
}

bool j1Player::Start()
{
	sprite = App->tex->Load(sprite_route.GetString());
	coll = App->collision->AddCollider(idle.GetCurrentFrame(), COLLIDER_PLAYER, this);
	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

bool j1Player::PreUpdate() 
{
	isGrounded = App->collision->CheckIfGrounded(coll);

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

	velocity = target_speed * acceleration + velocity * (1 - acceleration);
	if (fabs(velocity.x) < threshold) velocity.x = 0.0F;
	if (fabs(velocity.y) < threshold) velocity.y = 0.0F;

	new_position += velocity;
	coll->SetPos(new_position.x, new_position.y);

	return true;
}

bool j1Player::Update(float dt)
{
	position = new_position;
	coll->SetPos(position.x, position.y);

	CheckDeath();


	App->render->Blit(sprite, position.x, position.y, &current_animation.GetCurrentFrame(), 1.0F, flipX);	
	return true;
}

bool j1Player::CleanUp()
{
	if (sprite) 
	{
		App->tex->UnLoad(sprite);
		sprite = nullptr;
	}

	if (coll) 
	{
		coll->to_delete = true;
		coll = nullptr;
	}
	if (!isGrounded) state = JUMPING;

	return true;
}

void j1Player::IdleUpdate() 
{
	target_speed.x = 0.0F;
	if (App->input->GetKey(SDL_SCANCODE_D) != App->input->GetKey(SDL_SCANCODE_A))
	{
		state = MOVING;
		current_animation = move;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		target_speed.y = -jump_speed;
		state = JUMPING;
		current_animation = jump;
		isGrounded = false;
	}
	else if (!isGrounded) state = JUMPING;
}

void j1Player::MovingUpdate() 
{
	if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A))
	{
		state = IDLE;
		target_speed.x = 0.0F;
		current_animation = idle;
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		target_speed.y = -jump_speed;
		state = JUMPING;
		current_animation = jump;
		isGrounded = false;
	}
	else if (!isGrounded) state = JUMPING;
}

void j1Player::JumpingUpdate() 
{
	target_speed.y += gravity;

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
		if (App->input->GetKey(SDL_SCANCODE_D) == App->input->GetKey(SDL_SCANCODE_A))
		{
			state = IDLE;
			current_animation = idle;
		}
		else 
		{
			state = MOVING;
			current_animation = move;
		}
		target_speed.y = 0.0F;
		velocity.y = 0.0F;
	}
}

bool j1Player::OnCollision(Collider* c1, Collider* c2) 
{
	if (c2->type == COLLIDER_PLATFORM) 
	{
		if (velocity.x > 0 && (position.x + c1->rect.w) <= c2->rect.x) //RIGHT COLLISION
		{ 
			velocity.x = 0.0F;
			new_position.x = c2->rect.x - c1->rect.w;
		}
		else if (velocity.x < 0 && position.x >= (c2->rect.x + c2->rect.w)) //LEFT COLLISION
		{
			velocity.x = 0.0F;
			new_position.x = c2->rect.x+c2->rect.w;
		}
		if (position.y+c1->rect.h < c2->rect.y)  //BOTTOM COLLISION
		{
			velocity.y = 0.0F;
			new_position.y = c2->rect.y - c1->rect.h;
			isGrounded = true;
		}
	}
	return true;
}

bool j1Player::Load(pugi::xml_node &player) 
{
	position.x = player.child("position").attribute("x").as_float();
	position.y = player.child("position").attribute("y").as_float();

	return true;
}


bool j1Player::Save(pugi::xml_node &player) const
{
	pugi::xml_node pos = player.append_child("position");
	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Player::CheckDeath()
{
	if(position.y > App->map->data.height * App->map->data.tile_height && state != DEAD)
	{
		state = DEAD;
		current_animation = die;
		App->swap_scene->Reload();
		position = App->map->data.spawn;
	}
}