#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentAnimation.h"
#include "ComponentCamera.h"
#include "ComponentBone.h"
#include "ModuleSceneManager.h"

#include "Math.h"

#include "ResourceModel.h"

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
		TransformGlobal(this);

	/*ComponentAnimation* anim = this->GetComponent<ComponentAnimation>();
	if (GetComponent<ComponentAnimation>())
		anim->Update(dt);*/

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

void GameObject::RecursiveDelete(bool target)
{
	// --- Delete all childs of given GO, also destroys GO ---

	if (this->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = this->childs.begin(); it != this->childs.end(); ++it)
		{
			(*it)->RecursiveDelete(false);			
		}

		this->childs.clear();
	}
	// --- If this is the first object GO given to Recursive delete, erase it from parent's list ---
	if (target && this->parent)
		this->parent->RemoveChildGO(this);

	this->Static = true;
	App->scene_manager->SetStatic(this);
	App->scene_manager->tree.Erase(this);

	delete this;
}

//void GameObject::OnUpdateTransform()
//{
//	if (Static)
//		return;
//
//	ComponentTransform* transform = GetComponent<ComponentTransform>();
//
//	if(parent)
//		transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>()->GetGlobalTransform());
//
//	ComponentCamera* camera = GetComponent<ComponentCamera>();
//
//	if(camera)
//		camera->OnUpdateTransform(transform->GetGlobalTransform());
//
//	// --- Update all children ---
//	if (childs.size() > 0)
//	{
//		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
//		{
//			(*it)->OnUpdateTransform();	
//		}
//	}
//
//	UpdateAABB();
//}

void GameObject::TransformGlobal(GameObject* GO)
{
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	transform->OnUpdateTransform(GO->parent->GetComponent<ComponentTransform>()->GetGlobalTransform());

	for (std::vector<GameObject*>::iterator tmp = GO->childs.begin(); tmp != GO->childs.end(); ++tmp)
	{
		TransformGlobal(*tmp);
	}

}

void GameObject::RemoveChildGO(GameObject * GO)
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

void GameObject::AddChildGO(GameObject * GO)
{
	// --- Add a child GO to a Game Object this ---
	if (!FindChildGO(GO))
	{
		if (GO->parent)
			GO->parent->RemoveChildGO(GO);

		GO->parent = this;
		childs.push_back(GO);

		/*ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
		transform->SetGlobalTransform(this->GetComponent<ComponentTransform>()->GetGlobalTransform());*/
	}
}

bool GameObject::FindChildGO(GameObject * GO)
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

void GameObject::GetAllChilds(std::vector<GameObject*>& collector)
{
	collector.push_back(this);
	for (uint i = 0; i < childs.size(); i++)
		childs[i]->GetAllChilds(collector);
}

GameObject* GameObject::GetAnimGO(GameObject* GO)
{
	ComponentAnimation* anim = GO->GetComponent<ComponentAnimation>();

	if (anim != nullptr)
	{
		return anim->GetContainerGameObject();
	}
	else
	{
		if (GO->parent)
			return GetAnimGO(GO->parent);
		else
			return nullptr;
	}
		
}

Component * GameObject::AddComponent(Component::ComponentType type)
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
			case Component::ComponentType::Bone:
				component = new ComponentBone(this);
				break;
			case Component::ComponentType::Animation:
				component = new ComponentAnimation(this);
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

uint& GameObject::GetUID()
{
	return UID;
}

std::string GameObject::GetName() const
{
	return name;
}

const AABB & GameObject::GetAABB()
{
	UpdateAABB();
	return aabb;
}

const OBB & GameObject::GetOBB() const
{
	return obb;
}

bool & GameObject::GetActive()
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
	if(!mesh)
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


