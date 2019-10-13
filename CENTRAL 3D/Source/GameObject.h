#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include <list>
#include "Math.h"

class GameObject
{

public:

	GameObject(const char* name);
	virtual ~GameObject();

	// --- Getters ---
	uint GetUID() const;

	Component* AddComponent(Component::ComponentType type);

private:
	// Unique Identifier
	uint UID = 0;

public:
	float4x4 transform = math::float4x4::identity;
	std::list<Component*> components;
	std::string name;
};

#endif

