#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "JSONLoader.h"

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		Transform,
		Mesh,
		Renderer,
		Material,
		Camera,
		Unknown
	};

public:

	Component(GameObject* ContainerGO,Component::ComponentType type);
	Component(Component::ComponentType type);
	virtual ~Component();
	virtual void Enable();
	virtual void Disable();

	// --- Getters ---
	ComponentType GetType() const;
	GameObject* GetContainerGameObject() const;
	bool& GetActive();
	bool IsEnabled() const;

	// --- Save & Load ---
	virtual void Save(json& file) const = 0;

protected:
	bool active = false;
	GameObject* GO = nullptr;
	ComponentType type = ComponentType::Unknown;
};

#endif

