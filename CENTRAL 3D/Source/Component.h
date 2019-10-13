#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		Unknown = 0,
		Mesh,
		Renderer
	};

public:

	Component(GameObject* ContainerGO,Component::ComponentType type);
	virtual ~Component();

private:
	GameObject* GO = nullptr;
	Component::ComponentType type;
};

#endif

