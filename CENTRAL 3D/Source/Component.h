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
	virtual void Enable();
	virtual void Disable();

	// --- Getters ---
	Component::ComponentType GetType();
	GameObject* GetContainerGameObject();
	bool& GetActive();
	bool IsEnabled();

protected:
	bool active = false;
	GameObject* GO = nullptr;
	Component::ComponentType type;
};

#endif

