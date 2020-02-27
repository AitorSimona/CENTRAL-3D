#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentCamera.h"
#include "ComponentCollider.h"
#include "ComponentDynamicRigidBody.h"

#include "ComponentScript.h"
#include "ModuleSceneManager.h"

#include "Math.h"

#include "ResourceModel.h"
#include "ResourceScene.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	UID = App->GetRandom().Int();
	this->name = name;
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();

	Enable();
}

GameObject::GameObject(const char* name, uint UID)
{
	this->UID = UID;
	this->name = name;
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();

	Enable();
}

GameObject::~GameObject()
{
	// --- Destroy all components and game object ---

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
			delete *it;

	}
	components.clear();

	if (model)
		model->Release();
}

void GameObject::Update(float dt)
{
	if (GetComponent<ComponentTransform>()->update_transform)
		this->OnUpdateTransform();



	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->Update(dt);
	}

	// PHYSICS: TEMPORAL example, after updating transform, update collider
	ComponentCollider* collider = GetComponent<ComponentCollider>();

	if (collider) {
		collider->UpdateLocalMatrix();
	}

}

void GameObject::RecursiveDelete()
{
	// --- Delete all childs of given GO ---

	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			(*it)->RecursiveDelete();
			delete *it;
		}

		childs.clear();
	}

	std::unordered_map<uint, GameObject*>::iterator it;

	// --- If go is static eliminate it from octree and scene static go map ---
	if (Static)
	{
		it = App->scene_manager->currentScene->StaticGameObjects.find(UID);

		if (it != App->scene_manager->currentScene->StaticGameObjects.end())
			App->scene_manager->currentScene->StaticGameObjects.erase(UID);

		App->scene_manager->tree.Erase(this);
	}
	else // If it is not static just eliminate it from scene nostatic go map ---
	{
		it = App->scene_manager->currentScene->NoStaticGameObjects.find(UID);;

		if (it != App->scene_manager->currentScene->NoStaticGameObjects.end())
			App->scene_manager->currentScene->NoStaticGameObjects.erase(UID);
	}
}

void GameObject::OnUpdateTransform()
{
	if (Static)
		return;

	ComponentTransform* transform = GetComponent<ComponentTransform>();

	if (parent)
		transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>()->GetGlobalTransform());

	ComponentCamera* camera = GetComponent<ComponentCamera>();

	if (camera)
		camera->OnUpdateTransform(transform->GetGlobalTransform());

	// --- Update all children ---
	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			(*it)->OnUpdateTransform();
		}
	}

	UpdateAABB();
}

void GameObject::RemoveChildGO(GameObject* GO)
{
	// --- Remove given child from list ---
	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
		{
			if ((*go)->GetUID() == GO->GetUID())
			{
				childs.erase(go);
				break;
			}
		}
	}
}

void GameObject::AddChildGO(GameObject* GO)
{
	// --- Add a child GO to a Game Object this ---
	if (!FindChildGO(GO))
	{
		if (GO->parent)
			GO->parent->RemoveChildGO(GO);

		GO->parent = this;
		childs.push_back(GO);

		//ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
		//transform->SetGlobalTransform(this->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform());
	}
}

bool GameObject::FindChildGO(GameObject* GO)
{
	// --- Look for given GO in child list and return true if found ---
	bool ret = false;

	if (childs.size() > 0)
	{
		std::vector<GameObject*>::iterator go = childs.begin();

		for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
		{
			if ((*go)->GetUID() == GO->GetUID())
				ret = true;
		}
	}

	return ret;
}

Component* GameObject::AddComponent(Component::ComponentType type)
{
	BROKEN_ASSERT(static_cast<int>(Component::ComponentType::Unknown) == 6, "Component Creation Switch needs to be updated");

	Component* component = nullptr;

	// --- Check if there is already a component of the type given ---

	if (HasComponent(type) == nullptr)
	{
		switch (type)
		{
		case Component::ComponentType::Transform:
			component = new ComponentTransform(this);
			break;
		case Component::ComponentType::Mesh:
			component = new ComponentMesh(this);
			UpdateAABB();
			break;
		case Component::ComponentType::MeshRenderer:
			component = new ComponentMeshRenderer(this);
			break;
		case Component::ComponentType::Camera:
			component = new ComponentCamera(this);
			break;
		case Component::ComponentType::Collider:
			component = new ComponentCollider(this);
			break;
		case Component::ComponentType::DynamicRigidBody:
			component = new ComponentDynamicRigidBody(this);
			break;
		case Component::ComponentType::Script:
			component = new ComponentScript(this);
			break;
		}

		if (component)
			components.push_back(component);

	}
	else
	{
		// --- If we find a component of the same type, tell the user and return such component ---
		ENGINE_CONSOLE_LOG("![Warning]: The current Game Object already has a component of the type given");
		component = HasComponent(type);
	}

	return component;
}

void GameObject::RemoveComponent(Component::ComponentType type)
{
	// ---Remove component of type given from game object ---

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == type)
		{
			std::vector<Component*>::iterator it = components.begin();
			it += i;

			components.erase(it);

			break;
		}
	}
}

Component* GameObject::HasComponent(Component::ComponentType type) const
{
	// --- Search for given type of component ---
	Component* component = nullptr;

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}

std::vector<Component*>& GameObject::GetComponents()
{
	return components;
}

void GameObject::Enable()
{
	active = true;
}

void GameObject::Disable()
{
	active = false;
}

uint GameObject::GetUID()
{
	return UID;
}

void GameObject::SetUID(uint uid)
{
	// --- Try to eliminate go from current scene ---
	App->scene_manager->currentScene->NoStaticGameObjects.erase(UID);
	App->scene_manager->currentScene->StaticGameObjects.erase(UID);

	UID = uid;

	if (Static)
	{
		App->scene_manager->currentScene->StaticGameObjects[UID] = this;
	}
	else
	{
		App->scene_manager->currentScene->NoStaticGameObjects[UID] = this;

	}
}

std::string GameObject::GetName() const
{
	return name;
}

const AABB& GameObject::GetAABB()
{
	UpdateAABB();
	return aabb;
}

const OBB& GameObject::GetOBB() const
{
	return obb;
}

bool& GameObject::GetActive()
{
	return active;
}

bool GameObject::IsEnabled() const
{
	return active;
}

void GameObject::SetName(const char* name)
{
	if (name && name != "root")
		this->name = name;
}

void GameObject::UpdateAABB()
{
	ComponentMesh* mesh = GetComponent<ComponentMesh>();
	ComponentTransform* transform = GetComponent<ComponentTransform>();

	if (mesh)
	{
		obb = mesh->GetAABB();
		obb.Transform(transform->GetGlobalTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	if (!mesh)
	{
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(1, 1, 1));
		obb = aabb;
	}
}

void GameObject::ONResourceEvent(uint uid, Resource::ResourceNotificationType type)
{
	for (uint i = 0; i < components.size(); ++i)
	{
		components[i]->ONResourceEvent(uid, type);
	}

	if (model && type == Resource::ResourceNotificationType::Deletion && model->GetUID() == uid)
		model = nullptr;
}
