#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ModuleSceneManager.h"

#include "Math.h"


#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	UID = App->GetRandom().Int();
	this->name = name;
	Enable();
}

GameObject::~GameObject()
{
	// --- Destroy all components and game object ---

	ComponentMaterial* mat = GetComponent<ComponentMaterial>(Component::ComponentType::Material);

	if (mat && mat->resource_material && mat->resource_material->resource_diffuse)
		mat->resource_material->resource_diffuse->instances--;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
		{
			if ((*it)->GetType() != Component::ComponentType::Material)
				delete(*it);

			// MYTODO: We are not deleting materials here (should be deleted by user via project folder) All materials are deleted on app end

			*it = nullptr;
		}
	}
	components.clear();
}

void GameObject::Update(float dt)
{
	if (GetComponent<ComponentTransform>(Component::ComponentType::Transform)->update_transform)
		this->OnUpdateTransform();

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

void GameObject::OnUpdateTransform()
{
	if (Static)
		return;

	ComponentTransform* transform = GetComponent<ComponentTransform>(Component::ComponentType::Transform);

	if(parent)
	transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform());

	ComponentCamera* camera = GetComponent<ComponentCamera>(Component::ComponentType::Camera);

	if(camera)
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

		//ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
		//transform->SetGlobalTransform(this->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform());
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
			if (*go == GO)
				ret = true;
		}
	}

	return ret;
}

Component * GameObject::AddComponent(Component::ComponentType type)
{
	static_assert(static_cast<int>(Component::ComponentType::Unknown) == 5, "Component Creation Switch needs to be updated");

	Component* new_component = nullptr;

	// --- Check if there is already a component of the type given ---

	if (HasComponent(type) == false)
	{

		switch (type)
		{
		case Component::ComponentType::Transform:
			new_component = new ComponentTransform(this);
			break;
		case Component::ComponentType::Mesh:
			new_component = new ComponentMesh(this);
			UpdateAABB();
			break;
		case Component::ComponentType::Renderer:
			new_component = new ComponentRenderer(this);
			break;
		case Component::ComponentType::Camera:
			new_component = new ComponentCamera(this);
			break;
		}

		if (new_component)
			components.push_back(new_component);

	}
	else
	{
		// --- If we find a component of the same type, tell the user ---

		CONSOLE_LOG("|[error]: The current Game Object already has a component of the type given");

	}

	return new_component;
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

bool GameObject::HasComponent(Component::ComponentType type) const
{
	// --- Search for given type of component ---

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == type)
			return true;
	}

	return false;
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

const AABB & GameObject::GetAABB() const
{
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
	if (name)
		this->name = name;
}

void GameObject::SetMaterial(ComponentMaterial * material)
{
	if (material)
	{
		RemoveComponent(Component::ComponentType::Material);
		components.push_back(material);
	}
}

void GameObject::UpdateAABB()
{
	ComponentMesh* mesh = GetComponent<ComponentMesh>(Component::ComponentType::Mesh);
	ComponentTransform* transform = GetComponent<ComponentTransform>(Component::ComponentType::Transform);

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
