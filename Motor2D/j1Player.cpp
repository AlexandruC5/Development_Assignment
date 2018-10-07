#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
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

	//load player stats
	max_speed = player_config.child("speed").attribute("maxSpeed").as_float();
	acceleration = player_config.child("acceleration").attribute("value").as_float();
	return true;
}

bool j1Player::Update(float dt)
{
	switch (state) {
	case IDLE:
	{
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) target_speed.x = max_speed;
		else if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) target_speed.x = -max_speed;
		else target_speed.x = 0.0F;	
		

		velocity = target_speed * acceleration + velocity * (1 - acceleration);
		position += velocity;
	}
		break;
	case MOVING:
		break;
	case JUMP:
		break;
	case DEAD:
		break;
	default:
		break;
	}

	App->render->Blit(sprite, position.x, position.y, &idle.GetCurrentFrame());
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

	return true;
}
