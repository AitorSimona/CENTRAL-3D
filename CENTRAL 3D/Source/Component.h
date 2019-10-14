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
		Material,
		Unknown
	};

public:

	Component(GameObject* ContainerGO,Component::ComponentType type);
	Component(Component::ComponentType type);
	virtual ~Component();

	Component::ComponentType GetType();
	GameObject* GetContainerGameObject();

protected:
	GameObject* GO = nullptr;
	Component::ComponentType type;
};

#endif

