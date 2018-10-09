#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
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
	player_config = conf;

	return true;
}

bool j1Player::Start()
{
	//load spritesheet
	p2SString temp = player_config.child("folder").child_value();
	temp += player_config.child("sprite").child_value();
	sprite = App->tex->Load(temp.GetString());

	//load animations
	pugi::xml_node frame;
	for (frame = player_config.child("animations").child("idle").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		idle.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	idle.speed = player_config.child("animations").child("idle").attribute("speed").as_float();

	for (frame = player_config.child("animations").child("move").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		move.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	move.speed = player_config.child("animations").child("move").attribute("speed").as_float();

	for (frame = player_config.child("animations").child("jump").child("frame"); frame; frame = frame.next_sibling("frame"))
	{
		jump.PushBack({ frame.attribute("x").as_int(), frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
	}
	jump.speed = player_config.child("animations").child("jump").attribute("speed").as_float();
	jump.loop = false;

	//load player stats
	movement_speed = player_config.child("movement_speed").attribute("value").as_float();
	jump_speed = player_config.child("jump_speed").attribute("value").as_float();
	acceleration = player_config.child("acceleration").attribute("value").as_float();
	threshold = player_config.child("threshold").attribute("value").as_float();
	gravity = player_config.child("gravity").attribute("value").as_float();

	//player setup
	current_animation = idle;
	coll = App->collision->AddCollider(idle.GetCurrentFrame(), COLLIDER_PLAYER, this);
	return true;
}

bool j1Player::PostUpdate()
{
	App->render->Blit(sprite, position.x, position.y, &current_animation.GetCurrentFrame(), 1.0F, flipX);
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
	coll->SetPos(position.x + velocity.x, position.y + velocity.y); //move collider only to check for possible collisions

	return true;
}

bool j1Player::Update(float dt)
{
	position = new_position;
	coll->SetPos(position.x, position.y);
	
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
		if (velocity.x > 0 && (position.x + c1->rect.w) <= c2->rect.x) {
			velocity.x = 0.0F;
			new_position.x = c2->rect.x - c1->rect.w;
		}
		else if (velocity.x < 0 && position.x >= (c2->rect.x + c2->rect.w)) 
		{
			velocity.x = 0.0F;
			new_position.x = c2->rect.x+c2->rect.w;
		}
		if (position.y+c1->rect.h < c2->rect.y) 
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