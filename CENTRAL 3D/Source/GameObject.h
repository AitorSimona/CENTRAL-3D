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
	void Update(float dt);

	// --- Getters ---
	uint&			GetUID();
	std::string		GetName() const;
	const AABB&	    GetAABB() const;
	const OBB&      GetOBB() const;

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
	std::vector<Component*>& GetComponents();

	// --- Setters ---
	void			SetName(const char* name);
	void			SetMaterial(ComponentMaterial* material);

	// --- Utilities ---
	void RecursiveDelete(GameObject* GO, bool target = true);
	void OnUpdateTransform(GameObject* GO);
	void RemoveChildGO(GameObject* GO);
	void AddChildGO(GameObject* GO);
	bool FindChildGO(GameObject* GO);

private:
	void UpdateAABB();

public:
	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;
private:
	// Unique Identifier
	uint UID = 0;
	std::string name;
	std::vector<Component*> components;

	bool active = false;

	AABB						aabb;
	OBB							obb;
};

#endif
