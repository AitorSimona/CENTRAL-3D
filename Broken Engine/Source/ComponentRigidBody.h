#ifndef _COMPONENT_RIGIDBODY_H_
#define _COMPONENT_RIGIDBODY_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

using namespace physx;

class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(GameObject* ContainerGO);
	virtual ~ComponentRigidBody();

	inline PxReal GetMass() { return rigidBody->getMass(); }
	inline void SetMass(PxReal mass) { rigidBody->setMass(mass); }
	inline void SetDensity(PxReal density) { PxRigidBodyExt::updateMassAndInertia(*rigidBody, density); }

	inline void UseGravity(bool enable) { rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, enable); }
	inline void setKinematic(bool enable) { rigidBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); }

	inline void FeezePosition_X(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, enable); }
	inline void FeezePosition_Y(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, enable); }
	inline void FeezePosition_Z(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, enable); }
	inline void FreeseRotation_X(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, enable); }
	inline void FreeseRotation_Y(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, enable); }
	inline void FreeseRotation_Z(bool enable) { rigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, enable); }

	static inline Component::ComponentType GetType() { return Component::ComponentType::RigidBody; };

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	PxRigidDynamic* rigidBody = nullptr;
};
#endif
