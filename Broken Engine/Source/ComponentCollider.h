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
		BOX,
		SPHERE,
		CAPSULE,
		PLANE,
		NONE
	};

public:
	ComponentCollider(GameObject* ContainerGO);
	virtual ~ComponentCollider();

	void Draw() const;

	void SetPosition();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	void CreateCollider(ComponentCollider::COLLIDER_TYPE type);

	static inline Component::ComponentType GetType() { return Component::ComponentType::Collider; };

public:
	COLLIDER_TYPE type = COLLIDER_TYPE::NONE;
	ResourceMesh* mesh = nullptr;
	bool editCollider = false;
	float3 localPosition = float3::zero;

private:
	physx::PxShape* shape = nullptr;
	physx::PxTransform globalPosition;
	int lastIndex = -1;
};

#endif __COMPONENT_COLLIDER_H__