#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		Mesh,
		Renderer,
		Unknown
	};

public:

	Component(GameObject* ContainerGO,Component::ComponentType type);
	virtual ~Component();

	Component::ComponentType GetType();

private:
	GameObject* GO = nullptr;
	Component::ComponentType type;
};

#endif

