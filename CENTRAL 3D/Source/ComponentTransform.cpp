#include "ComponentTransform.h"

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
	float3 rotation = float3::zero;

	//rotation.x = Atan2(Local_transform.ptr()[2][0], Local_transform.ptr()[2][0]);


	return rotation;
}

float4x4 ComponentTransform::GetLocalTransform() const
{
	return Local_transform;
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	position = float3(x, y, z);

	Local_transform.ptr()[12] = position.x;
	Local_transform.ptr()[13] = position.y;
	Local_transform.ptr()[14] = position.z;
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
	//Local_transform.Scale(x, y, z);

	scale = float3(x, y, z);

	Local_transform.ptr()[0] = scale.x;
	Local_transform.ptr()[5] = scale.y;
	Local_transform.ptr()[10] = scale.z;
}

void ComponentTransform::SetLocalTransform(float4x4 new_transform)
{
	Local_transform = new_transform;
}

void ComponentTransform::UpdateLocalTransform()
{
	Local_transform = float4x4::FromTRS(position, rotation, scale);
}

void ComponentTransform::UpdateTRS()
{
	Local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ()*RADTODEG;
}
