#ifndef __COMPONENT_COLLIDER_H__
#define __COMPONENT_COLLIDER_H__

#include "Component.h"
#include "Math.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

class ResourceMesh;

class ComponentCollider : public Component
{
public:
	enum class COLLIDER_TYPE
	{
		NONE,
		BOX,
		SPHERE,
		CAPSULE,
		PLANE
	};

public:
	ComponentCollider(GameObject* ContainerGO);
	virtual ~ComponentCollider();

	void Draw();

	void UpdateLocalMatrix();

	void SetPosition();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	void CreateCollider(ComponentCollider::COLLIDER_TYPE type, bool createAgain = false);

	static inline Component::ComponentType GetType() { return Component::ComponentType::Collider; };

private:
	template <class Geometry>
	bool HasDynamicRigidBody(Geometry geometry, physx::PxTransform transform) const;

public:
	COLLIDER_TYPE type = COLLIDER_TYPE::NONE;
	ResourceMesh* mesh = nullptr;
	bool editCollider = false;
	float3 localPosition = float3::zero;
	float3 centerPosition = float3::zero;
	float3 baseScale = float3::one;
	float3 originalScale = float3::one;
	physx::PxRigidStatic* rigidStatic = nullptr;
	float3 offset = float3::zero;

private:
	physx::PxShape* shape = nullptr;
	float3 globalPosition;
	float3 scale = float3(1, 1, 1);
	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;
	float radius = 1.0f, height = 1.0f;
	int lastIndex = -1;
	int colliderType = 0;
	float3 tmpScale = float3::one;
	bool firstCreation = false;
};

#endif __COMPONENT_COLLIDER_H__
