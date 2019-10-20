#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"

#include "Math.h"


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
	//Local_transform.Translate(x,y,z);
	//Local_transform.SetTranslatePart(x, y, z);
	//Local_transform.SetRow3(
	Local_transform.ptr()[12] = x;
	Local_transform.ptr()[13] = y;
	Local_transform.ptr()[14] = z;
}

void GameObject::SetRotationAxisAngle(const float3 & rot_axis, float degrees_angle)
{
	//Local_transform.RotateAxisAngle(rot_axis, degrees_angle);

		// --- Angle in RADIANS ---
	degrees_angle = degrees_angle / 180.0f * (float)pi;

	// --- Normalize vector ---
	float3 v = rot_axis.Normalized();

	float c = 1.0f - cosf(degrees_angle), s = sinf(degrees_angle);

	Local_transform.ptr()[0] = 1.0f + c * (v.x * v.x - 1.0f);
	Local_transform.ptr()[1] = c * v.x * v.y + v.z * s;
	Local_transform.ptr()[2] = c * v.x * v.z - v.y * s;
	Local_transform.ptr()[4] = c * v.x * v.y - v.z * s;
	Local_transform.ptr()[5] = 1.0f + c * (v.y * v.y - 1.0f);
	Local_transform.ptr()[6] = c * v.y * v.z + v.x * s;
	Local_transform.ptr()[8] = c * v.x * v.z + v.y * s;
	Local_transform.ptr()[9] = c * v.y * v.z - v.x * s;
	Local_transform.ptr()[10] = 1.0f + c * (v.z * v.z - 1.0f);
}

void GameObject::Scale(float x, float y, float z)
{
	//Local_transform.Scale(x, y, z);

	Local_transform.ptr()[0] = x;
	Local_transform.ptr()[5] = y;
	Local_transform.ptr()[10] = z;
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
