#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"

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
			if((*it)->GetType() != Component::ComponentType::Material)
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
	Local_transform.TransformPos(position);
	Local_transform.TranslatePart();

	return position;
}

float4x4 GameObject::GetLocalTransform()
{
	return Local_transform;
}

Component * GameObject::GetComponent(Component::ComponentType type)
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			return *it;
		}
	}

	return nullptr;
}

Component * GameObject::AddComponent(Component::ComponentType type)
{
	static_assert(static_cast<int>(Component::ComponentType::Unknown) == 3, "Component Creation Switch needs to be updated");

	Component* new_component = nullptr;

	// --- Check if there is already a component of the type given ---

	if (GetComponent(type) == nullptr)
	{

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

	}
	else
	{
		// --- If we find a component of the same type, tell the user ---

		LOG("|[error]: The current Game Object already has a component of the type given");

	}

	return new_component;
}

void GameObject::SetPosition(float x, float y, float z)
{
	Local_transform.Translate(x,y,z);
}

void GameObject::SetRotationAxisAngle(const float3 & rot_axis, float degrees_angle)
{
	Local_transform.RotateAxisAngle(rot_axis, degrees_angle);
}

void GameObject::Scale(float x, float y, float z)
{
	Local_transform.Scale(x, y, z);
}

void GameObject::SetLocalTransform(float4x4 new_transform)
{
	Local_transform = new_transform;
}

void GameObject::SetMaterial(ComponentMaterial * material)
{
	if (material)
		components.push_back(material);
}
