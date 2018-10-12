#ifndef __ModuleCollision_H__
#define __ModuleCollision_H__

#include "j1Module.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_FLOOR,
	COLLIDER_PLATFORM,
	COLLIDER_TRIGGER,
	COLLIDER_PLAYER,
	COLLIDER_MAX,
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	bool enabled = true;

	Collider(){}
	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	void SetShape(int w, int h)
	{
		rect.w = w;
		rect.h = h;
	}

	void SetType(COLLIDER_TYPE type)
	{
		this->type = type;
	}
	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool Update(float dt) override;
	bool CleanUp() override;
	bool PreUpdate();
	bool Awake(pugi::xml_node&);


	Collider* ClosestRightSideCollider(Collider* coll) const;
	Collider* ClosestLeftSideCollider(Collider* coll) const;
	Collider* ClosestBottomSideCollider(Collider* coll) const;
	Collider* ClosestTopSideCollider(Collider* coll) const;
	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);

	void DebugDraw();

private:

	Collider** colliders = nullptr;
	bool debug = false;
	int max_colliders = 0;
};

#endif
