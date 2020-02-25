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

	void SetPosition();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	void CreateCollider(ComponentCollider::COLLIDER_TYPE type, bool createAgain = false);

	static inline Component::ComponentType GetType() { return Component::ComponentType::Collider; };

private:
	bool HasDynamicRigidBody(physx::PxGeometry geometry) const;
	bool HasDynamicRigidBody() const;

public:
	COLLIDER_TYPE type = COLLIDER_TYPE::NONE;
	ResourceMesh* mesh = nullptr;
	bool editCollider = false;
	float3 localPosition = float3::zero;

private:
	physx::PxShape* shape = nullptr;
	physx::PxRigidStatic* rigidStatic = nullptr;
	float3 globalPosition;
	float3 scale = float3::one;
	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;

	float radius = 1.0f, height = 1.0f;
	int lastIndex = -1;
};

#endif __COMPONENT_COLLIDER_H__