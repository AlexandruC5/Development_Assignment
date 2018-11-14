#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Entity.h"

j1Entity::j1Entity(EntityType type)
{}

j1Entity::~j1Entity()
{}

bool j1Entity::Awake(pugi::xml_node &)
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
}
