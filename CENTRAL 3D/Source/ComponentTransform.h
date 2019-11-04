#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"
#include "Math.h"

class ComponentTransform : public Component
{
public:

	ALIGN_CLASS_TO_16

	ComponentTransform(GameObject* ContainerGO);
	virtual ~ComponentTransform();

	// --- Getters ---
	float3			GetPosition() const;
	float3			GetScale() const;
	float3			GetRotation() const;
	float4x4        GetLocalTransform() const;

	// --- Setters ---
	void			SetPosition(float x, float y, float z);
	void			SetRotation(float3 euler_angles);
	void			Scale(float x, float y, float z);
	void			SetLocalTransform(float4x4 new_transform);

private:
	void UpdateLocalTransform();
	void UpdateTRS();

private:
	float4x4 Local_transform = math::float4x4::identity;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3	rotation_euler = float3::zero;
	float3 scale = float3::one;
};

#endif