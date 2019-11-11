#include "ComponentTransform.h"

#include "GameObject.h"

ComponentTransform::ComponentTransform(GameObject * ContainerGO) : Component(ContainerGO, Component::ComponentType::Transform)
{
}

ComponentTransform::~ComponentTransform()
{
}

float3 ComponentTransform::GetPosition() const
{
	return position;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

float3 ComponentTransform::GetRotation() const
{
	return rotation_euler;
}

float4x4 ComponentTransform::GetLocalTransform() const
{
	return Local_transform;
}

float4x4 ComponentTransform::GetGlobalTransform() const
{
	return Global_transform;
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	position = float3(x, y, z);

	UpdateLocalTransform();
}

void ComponentTransform::SetRotation(float3 euler_angles)
{
	// --- Compute desired rotation in radians ---
	float3 difference = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quatrot = Quat::FromEulerXYZ(difference.x, difference.y, difference.z);

	// --- Update own variables ---
	rotation = rotation * quatrot;
	rotation_euler = euler_angles;

	// --- Update Transform ---
	UpdateLocalTransform();
}

void ComponentTransform::Scale(float x, float y, float z)
{
	scale = float3(x, y, z);
	UpdateLocalTransform();
}

void ComponentTransform::SetGlobalTransform(float4x4 new_transform)
{
	Local_transform = GO->parent->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform().Inverted()*new_transform;
	Global_transform = new_transform;
	update_transform = true;
}

void ComponentTransform::UpdateLocalTransform()
{
	Local_transform = float4x4::FromTRS(position, rotation, scale);
	update_transform = true;
}

void ComponentTransform::OnUpdateTransform(const float4x4 & ParentGlobal)
{
	Global_transform = ParentGlobal * Local_transform;
	UpdateTRS();

	update_transform = false;
}

void ComponentTransform::UpdateTRS()
{
	Local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ()*RADTODEG;
}
