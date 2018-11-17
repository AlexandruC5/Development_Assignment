#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Entity.h"

j1Entity::j1Entity(EntityType type, pugi::xml_node config)
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
	return false;
}

bool j1Entity::Load(pugi::xml_node &)
{
	return false;
}

bool j1Entity::Save(pugi::xml_node &) const
{
	return false;
}

void j1Entity::CheckDeath()
{
}

void j1Entity::SetPosition(float x, float y)
{
	position = { x,y };
	if (collider) collider->SetPos(position.x, position.y);
}
