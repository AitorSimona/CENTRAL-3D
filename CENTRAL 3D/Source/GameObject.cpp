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

float3 GameObject::GetPosition()
{
	float3 position; 
	transform.TransformPos(position);
	transform.TranslatePart();

	return position;
}

float4x4 GameObject::GetTransform()
{
	return transform;
}

Component * GameObject::GetComponent(Component::ComponentType type)
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it))
		{
			delete(*it);
			*it = nullptr;
		}
	}

	return nullptr;
}

Component * GameObject::AddComponent(Component::ComponentType type)
{
	static_assert(static_cast<int>(Component::ComponentType::Unknown) == 2, "Component Creation Switch needs to be updated");

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

void GameObject::SetPosition(float x, float y, float z)
{
	transform.Translate(x,y,z);
}

void GameObject::SetRotationAxisAngle(const float3 & rot_axis, float degrees_angle)
{
	transform.RotateAxisAngle(rot_axis, degrees_angle);
}

void GameObject::Scale(float x, float y, float z)
{
	transform.Scale(x, y, z);
}

void GameObject::SetTransform(float4x4 new_transform)
{
	transform = new_transform;
}
