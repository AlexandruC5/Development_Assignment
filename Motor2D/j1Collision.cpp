#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Collision.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_PLATFORM][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_PLAYER] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_PLATFORM] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) && c1->enabled)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}

// Called before render is available
bool j1Collision::Update(float dt)
{
	DebugDraw();

	return true;
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_PLATFORM: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha, true);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha, true);
			break;
		default:
			break;
		}
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

bool j1Collision::CheckIfGrounded(Collider * c1) const
{
	SDL_Rect bottom_collider = c1->rect;
	bottom_collider.h += 1;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		if (colliders[i] != c1 && colliders[i] != nullptr && colliders[i]->CheckCollision(bottom_collider)) return true;
	return false;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r, COLLISION_SIDE side) const
{
	if (enabled) {
		if ((r.x < rect.x + rect.w) && (rect.x < r.x + r.w)
			&& (r.y < rect.y + rect.h) && (rect.y < r.y + r.h)) {

			/*if ((r.x < rect.x + rect.w) && (r.x + r.w > rect.x + rect.w)) side = LEFT_COLLISION;
			if (r.y+r.h<rect.y && r)*/
			return true;
		}
		return false;
	}
	else return false;
}