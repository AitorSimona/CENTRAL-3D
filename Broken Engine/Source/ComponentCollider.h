#ifndef __COMPONENT_COLLIDER_H__
#define __COMPONENT_COLLIDER_H__

#include "Component.h"
#include "Math.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

class ComponentCollider : public Component
{
public:
	enum class COLLIDER_TYPE
	{
		BOX,
		SPHERE,
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

	static inline Component::ComponentType GetType() { return Component::ComponentType::Collider; };

public:
	COLLIDER_TYPE type = COLLIDER_TYPE::NONE;

	bool editCollider = false;
	float3 localPosition = float3::zero;

private:
	physx::PxRigidDynamic* box = nullptr;
	physx::PxTransform globalPosition;
};

#endif __COMPONENT_COLLIDER_H__