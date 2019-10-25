#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(GameObject * ContainerGO) : Component(ContainerGO, Component::ComponentType::Transform)
{
}

ComponentTransform::~ComponentTransform()
{
}

float3 ComponentTransform::GetPosition()
{
	float3 position;
	/*Local_transform.TransformPos(position);
	Local_transform.TranslatePart();*/

	position.x = Local_transform.ptr()[12];
	position.y = Local_transform.ptr()[13];
	position.z = Local_transform.ptr()[14];


	return position;
}

float3 ComponentTransform::GetScale()
{
	float3 scale;

	scale.x = Local_transform.ptr()[0];
	scale.y = Local_transform.ptr()[5];
	scale.z = Local_transform.ptr()[10];

	return scale;
}

float3 ComponentTransform::GetRotation()
{
	float3 rotation = float3::zero;

	//rotation.x = Atan2(Local_transform.ptr()[2][0], Local_transform.ptr()[2][0]);


	return rotation;
}

float4x4 ComponentTransform::GetLocalTransform()
{
	return Local_transform;
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	//Local_transform.Translate(x,y,z);
	//Local_transform.SetTranslatePart(x, y, z);
	//Local_transform.SetRow3(
	Local_transform.ptr()[12] = x;
	Local_transform.ptr()[13] = y;
	Local_transform.ptr()[14] = z;
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

	Local_transform.ptr()[0] = x;
	Local_transform.ptr()[5] = y;
	Local_transform.ptr()[10] = z;
}

void ComponentTransform::SetLocalTransform(float4x4 new_transform)
{
	Local_transform = new_transform;
}