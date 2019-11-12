#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"

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

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
		{
			if ((*it)->GetType() != Component::ComponentType::Material)
				delete(*it);

			*it = nullptr;
		}
	}
}

void GameObject::Update(float dt)
{
	if (GetComponent<ComponentTransform>(Component::ComponentType::Transform)->update_transform)
		OnUpdateTransform(this);

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->Update(dt);
	}

}

void GameObject::RecursiveDelete(GameObject* GO)
{
	// --- Delete all childs of given GO, also destroys GO ---
	if (GO->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = GO->childs.begin(); it != GO->childs.end(); ++it)
		{
			RecursiveDelete(*it);			
		}

		GO->childs.clear();
	}

	delete GO;
}

void GameObject::OnUpdateTransform(GameObject* GO)
{
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
	transform->OnUpdateTransform(GO->parent->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform());

	// --- Update all children ---
	if (GO->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = GO->childs.begin(); it != GO->childs.end(); ++it)
		{
			OnUpdateTransform(*it);
		}
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

		ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);

		if (transform)
			transform->SetGlobalTransform(this->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform());
		else
			LOG("|[error]: no transform");
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
	static_assert(static_cast<int>(Component::ComponentType::Unknown) == 4, "Component Creation Switch needs to be updated");

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
			break;
		case Component::ComponentType::Renderer:
			new_component = new ComponentRenderer(this);
			break;
		}

		if (new_component)
			components.push_back(new_component);

	}
	else
	{
		// --- If we find a component of the same type, tell the user ---

		LOG("|[error]: The current Game Object already has a component of the type given");

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

void GameObject::Enable()
{
	active = true;
}

void GameObject::Disable()
{
	active = false;
}

uint GameObject::GetUID() const
{
	return UID;
}

std::string GameObject::GetName() const
{
	return name;
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
