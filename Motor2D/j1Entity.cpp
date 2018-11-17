#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Entity.h"

j1Entity::j1Entity(EntityType type, pugi::xml_node config, fPoint position, p2SString id)
{
	this->type = type;

	sprite_route = PATH(config.child("folder").child_value(), config.child("sprite").child_value());
	sprite = App->tex->Load(sprite_route.GetString());

	movement_speed = config.child("movement_speed").attribute("value").as_float();
	jump_speed = config.child("jump_speed").attribute("value").as_float();
	acceleration = config.child("acceleration").attribute("value").as_float();
	threshold = config.child("threshold").attribute("value").as_float();
	gravity = config.child("gravity").attribute("value").as_float();
	fall_speed = config.child("fall_speed").attribute("value").as_float();

	collider_offset = config.child("collider").attribute("offset").as_int();
	this->position = position;

	this->id = id;
}

j1Entity::~j1Entity()
{}

bool j1Entity::Awake()
{
	return false;
}

bool j1Entity::Start()
{
	return false;
}

bool j1Entity::Update(float dt)
{
	return false;
}

bool j1Entity::PreUpdate()
{
	return false;
}

void j1Entity::IdleUpdate()
{
}

void j1Entity::MovingUpdate()
{
}

void j1Entity::JumpingUpdate()
{
}

void j1Entity::Jump()
{
}

void j1Entity::LoadAnimations(pugi::xml_node config)
{
	//load animations
	int index = 0;
	pugi::xml_node animation;
	for (animation = config.child("animations").first_child(); animation; animation = animation.next_sibling())
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

	animation_frame = { 0, 0, config.child("collider").attribute("width").as_int(), config.child("collider").attribute("height").as_int() };
}

void j1Entity::StepX(float dt)
{
}

void j1Entity::StepY(float dt)
{
}

bool j1Entity::CleanUp()
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

	return true;
}

bool j1Entity::Load(pugi::xml_node &data)
{
	position.x = data.child("position").attribute("x").as_float();
	position.y = data.child("position").attribute("y").as_float();

	velocity.x = data.child("velocity").attribute("x").as_float();
	velocity.y = data.child("velocity").attribute("y").as_float();

	target_speed.x = data.child("target_speed").attribute("x").as_float();
	target_speed.y = data.child("target_speed").attribute("y").as_float();

	state = (EntityState)data.child("state").attribute("value").as_int();
	is_grounded = data.child("is_grounded").attribute("value").as_bool();
	flipX = data.child("flipX").attribute("value").as_bool();

	collider->SetPos(position.x, position.y + collider_offset);
	return true;
}

bool j1Entity::Save(pugi::xml_node &conf) const
{
	//save position of entity
	pugi::xml_node position_node = conf.append_child("position");
	position_node.append_attribute("x") = position.x;
	position_node.append_attribute("y") = position.y;

	//save current speed of entity
	pugi::xml_node velocity_node = conf.append_child("velocity");
	velocity_node.append_attribute("x") = velocity.x;
	velocity_node.append_attribute("y") = velocity.y;

	pugi::xml_node target_speed_node = conf.append_child("target_speed");
	target_speed_node.append_attribute("x") = target_speed.x;
	target_speed_node.append_attribute("y") = target_speed.y;

	//save state of entity
	conf.append_child("state").append_attribute("value") = state != DEAD ? (int)state : (int)IDLE;
	conf.append_child("is_grounded").append_attribute("value") = is_grounded;
	conf.append_child("flipX").append_attribute("value") = flipX;

	return true;
}

void j1Entity::CheckDeath()
{
}

void j1Entity::SetPosition(float x, float y)
{
	position = { x,y };
	if (collider) collider->SetPos(position.x, position.y);
}
