#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Math.h"
#include "Component.h"

 
BE_BEGIN_NAMESPACE
class BROKEN_API ComponentCamera : public Component {
public:

	ComponentCamera(GameObject* ContainerGO);
	virtual ~ComponentCamera();

	// --- Getters ---
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;
	float4x4 GetOpenGLViewMatrix();
	float4x4 GetOpenGLProjectionMatrix();
	const float3 GetCameraPosition() const { return frustum.Pos(); }

	// --- Setters ---
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFOV(float fov);
	void SetAspectRatio(float ar);

	void SetCameraValues(float4 values);

	void Update() override;
	void DrawComponent() override;

	
	// --- Utilities ---
	void Look(const float3& position);
	void OnUpdateTransform(const float4x4& global);

	bool ContainsAABB(const AABB& ref);
	float2 WorldToScreen(const float3& pos);
	float3 ScreenToWorld(const float2& pos, float distance);

	static inline Component::ComponentType GetType() { return Component::ComponentType::Camera; };

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:

	Frustum		frustum;
	bool		culling = false;
	bool		active_camera = false;

};

BE_END_NAMESPACE
#endif