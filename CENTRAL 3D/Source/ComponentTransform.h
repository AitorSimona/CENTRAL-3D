#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"
#include "Math.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* ContainerGO);
	virtual ~ComponentTransform();

	// --- Getters ---
	float3			GetPosition() const;
	float3			GetScale() const;
	float3			GetRotation() const;
	float4x4        GetLocalTransform() const;

	// --- Setters ---
	void			SetPosition(float x, float y, float z);
	void			SetRotationAxisAngle(const float3 &rot_axis, float degrees_angle);
	void			Scale(float x, float y, float z);
	void			SetLocalTransform(float4x4 new_transform);

private:
	float4x4 Local_transform = math::float4x4::identity;
};

#endif