#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	UID = App->GetRandom().Int();
	this->name = name;
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
		{
			delete(*it);
			*it = nullptr;
		}
	}
}

uint GameObject::GetUID() const
{
	return UID;
}

Component * GameObject::AddComponent(Component::ComponentType type)
{
	if (type == Component::ComponentType::Unknown)
		LOG("|[error]: Invalid type of Component");

	Component* new_component = nullptr;

	switch (type)
	{
	case Component::ComponentType::Mesh:
		new_component = new ComponentMesh(this);
		break;
	case Component::ComponentType::Renderer:
		new_component = new ComponentRenderer(this);
		break;

	}

	if (new_component)
		components.push_back(new_component);

	return new_component;
}
