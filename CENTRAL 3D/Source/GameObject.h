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

	template<typename TComponent>
	TComponent*	GetComponent(Component::ComponentType type)
	{

		for (uint i = 0; i < components.size(); ++i)
		{
			if (components[i]->GetType() == type)
			{
				return ((TComponent*)(components[i]));
			}
		}
		
		return nullptr;
	}

	Component*		AddComponent(Component::ComponentType type);
	void			RemoveComponent(Component::ComponentType type);
	bool			HasComponent(Component::ComponentType type) const;

	// --- Setters ---
	void			SetName(const char* name);
	void			SetMaterial(ComponentMaterial* material);

	// --- Utilities ---
	void RecursiveDelete(GameObject* GO);
	void RemoveChildGO(GameObject* GO);

public:
	GameObject* parent = nullptr;
private:
	// Unique Identifier
	uint UID = 0;
	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;

	bool active = false;

};

#endif
