#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include "Math.h"
#include <vector>

class ComponentMaterial;

class GameObject
{

public:

	GameObject(const char* name);
	virtual ~GameObject();
	void Enable();
	void Disable();

	// --- Getters ---
	uint			GetUID() const;
	std::string		GetName() const;

	bool&			GetActive();
	bool			IsEnabled() const;

	// --- Components ---
	Component*		AddComponent(Component::ComponentType type);

	template<typename TComponent>
	TComponent*	GetComponent(Component::ComponentType type)
	{
		//Component::ComponentType type = TComponent::GetType();

		if (active)
		{
			for (uint i = 0; i < components.size(); ++i)
			{
				if (components[i]->GetType() == type)
				{
					return ((TComponent*)(components[i]));
				}
			}
		}
		return nullptr;
	}

	// --- Setters ---
	void			SetName(const char* name);
	void			SetMaterial(ComponentMaterial* material);

private:
	// Unique Identifier
	uint UID = 0;
	std::string name;
	std::vector<Component*> components;

	bool active = false;

};

#endif
