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

void ComponentTransform::SetRotationAxisAngle(const float3 & rot_axis, float degrees_angle)
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