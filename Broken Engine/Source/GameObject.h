#ifndef __BE_GAMEOBJECT_H__
#define __BE_GAMEOBJECT_H__

#include "Math.h"
#include "BrokenCore.h"
#include "Component.h"
#include <vector>
#include <string>
#include "Resource.h"

BE_BEGIN_NAMESPACE

class ResourceModel;

class BROKEN_API GameObject {

public:

	GameObject(const char* name);
	GameObject(const char* name, uint UID);
	virtual ~GameObject();
	void Enable();	
	void Disable();
	void Update(float dt);
	void PostUpdate();
	void Draw();

	// --- Getters ---
	uint&			GetUID();
	void			SetUID(uint uid);
	const char*		GetName() const;
	const AABB&	    GetAABB();
	const OBB&      GetOBB() const;
	int GetChildGOIndex(GameObject* GO);

	bool& GetActive();
	int GetLayer();
	bool			IsEnabled() const;

	// --- Components ---

	template<typename TComponent>
	TComponent* GetComponent() {

		for (uint i = 0; i < components.size(); ++i)
		{
		if (components[i] && components[i]->GetType() == TComponent::GetType())
				return ((TComponent*)(components[i]));
		}

		return nullptr;
	}


	Component*		AddComponent(Component::ComponentType type, int index = -1);
	void			RemoveComponent(Component* comp);
	Component*		HasComponent(Component::ComponentType type) const;
	std::vector<Component*>& GetComponents();
	Component* GetComponentWithUID(uint UUID);

	// --- Setters ---
	void SetName(const char* name);

	// --- Utilities ---
	void RecursiveDelete();
	//void OnUpdateTransform();
	void TransformGlobal(GameObject* GO);
	void RemoveChildGO(GameObject* GO);
	// If there is a GO at index, it will erase it and replace it
	void AddChildGO(GameObject* GO, int index = -1);
	// This will not erase the GO at index, just displace it and everything after it
	void InsertChildGO(GameObject* GO, int index);
	bool FindChildGO(GameObject* GO);
	bool FindParentGO(GameObject* GO);
	void GetAllChilds(std::vector<GameObject*>& collector);
	GameObject* GetAnimGO(GameObject* GO);

	void UpdateAABB();

	void ONResourceEvent(uint uid, Resource::ResourceNotificationType type);

public:
	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;
	std::vector<GameObject*> collisions;
	bool Static = false;
	bool navigationStatic = false;
	uint navigationArea = 0; 
	ResourceModel* model = nullptr;
	int layer;
	bool is_prefab_child = false;
	bool is_prefab_instance = false;

	// to avoid including ImGui header, it's hardcoded the ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth flags (128 and 2048 respectively)
	int node_flags = 2176;

	int index = -1;

private:
	// Unique Identifier
	uint UID = 0;
	std::string name;
	std::vector<Component*> components;

	bool active = false;
	AABB						aabb;
	OBB							obb;
};

BE_END_NAMESPACE

#endif
