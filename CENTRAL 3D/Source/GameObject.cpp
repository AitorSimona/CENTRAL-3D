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

	Disable();

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
		{
			if((*it)->GetType() != Component::ComponentType::Material)
			delete(*it);

			*it = nullptr;
		}
	}
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
