#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include "Math.h"
#include <vector>
#include "Resource.h"

class ResourceModel;

class GameObject
{

public:

	GameObject(const char* name);
	GameObject(const char* name, uint UID);
	virtual ~GameObject();
	void Enable();
	void Disable();
	void Update(float dt);
	void Draw();

	// --- Getters ---
	uint&			GetUID();
	void			SetUID(uint uid);
	std::string		GetName() const;
	const AABB&	    GetAABB();
	const OBB&      GetOBB() const;

	bool&			GetActive();
	bool			IsEnabled() const;

	// --- Components ---

	template<typename TComponent>
	TComponent*	GetComponent()
	{

		for (uint i = 0; i < components.size(); ++i)
		{
			if (components[i] && components[i]->GetType() == TComponent::GetType())
			{
				return ((TComponent*)(components[i]));
			}
		}
		
		return nullptr;
	}

	Component*		AddComponent(Component::ComponentType type, int index = -1);
	void			RemoveComponent(Component::ComponentType type);
	Component*		HasComponent(Component::ComponentType type) const;
	std::vector<Component*>& GetComponents();

	// --- Setters ---
	void			SetName(const char* name);

	// --- Utilities ---
	void RecursiveDelete(bool target = true);
	void OnUpdateTransform();
	void RemoveChildGO(GameObject* GO);
	void AddChildGO(GameObject* GO, int index = -1); // note that specifying index will remove any go at index
	void InsertChildGO(GameObject* GO, int index);
	bool FindChildGO(GameObject* GO);

	void UpdateAABB();

	void ONResourceEvent(uint uid, Resource::ResourceNotificationType type);

public:
	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;
	bool Static = false;
	ResourceModel* model = nullptr;
	int index = -1;
	bool is_prefab_child = false;
	bool is_prefab_instance = false;

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
