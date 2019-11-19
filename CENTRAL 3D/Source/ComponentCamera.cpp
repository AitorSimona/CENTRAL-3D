#include "ComponentCamera.h"


ComponentCamera::ComponentCamera(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Camera)
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 2000.0f;
	frustum.verticalFov = DEGTORAD * 60.0f;
	SetAspectRatio(1.3f);

	update_projection = true;
}

ComponentCamera::~ComponentCamera()
{
}

float ComponentCamera::GetNearPlane() const
{
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFarPlane() const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFOV() const
{
	return frustum.verticalFov * RADTODEG;
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

float4x4 ComponentCamera::GetOpenGLViewMatrix()
{
	math::float4x4 matrix = frustum.ViewMatrix();
	return matrix.Transposed();
}

float4x4 ComponentCamera::GetOpenGLProjectionMatrix()
{
	math::float4x4 matrix = frustum.ProjectionMatrix();
	return matrix.Transposed();
}

void ComponentCamera::SetNearPlane(float distance)
{
	if (distance > 0 && distance < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = distance;
		update_projection = true;
	}

}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance > 0 && distance > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = distance;
		update_projection = true;
	}
}

void ComponentCamera::SetFOV(float fov)
{
	float aspect_ratio = frustum.AspectRatio();

	frustum.verticalFov = DEGTORAD * fov;
	SetAspectRatio(aspect_ratio);
}

void ComponentCamera::SetAspectRatio(float ar)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ar);
	update_projection = true;
}


void ComponentCamera::Look(const float3 & position)
{
	float3 vector = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, vector.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

void ComponentCamera::OnUpdateTransform(const float4x4 & global)
{
	frustum.front = global.WorldZ();
	frustum.up = global.WorldY();

	float3 position = float3::zero;
	float3 scale = float3::one;
	Quat quat = Quat::identity;
	global.Decompose(position, quat, scale);

	frustum.pos = position;

	update_projection = true;
}
