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
	float4x4        GetGlobalTransform() const;
	float3		    GetGlobalPosition() const;

	// --- Setters ---
	void			SetPosition(float x, float y, float z);
	void			SetRotation(float3 euler_angles);
	void			Scale(float x, float y, float z);
	void			SetGlobalTransform(float4x4 new_transform);
	void			OnUpdateTransform(const float4x4& ParentGlobal);

public:
	bool update_transform = false;
private:
	void UpdateLocalTransform();
	void UpdateTRS();

private:
	float4x4 Local_transform = math::float4x4::identity;
	float4x4 Global_transform = math::float4x4::identity;

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3	rotation_euler = float3::zero;
	float3 scale = float3::one;
};

#endif