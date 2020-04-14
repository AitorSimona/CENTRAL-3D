#ifndef _COMPONENT_RIGIDBODY_H_
#define _COMPONENT_RIGIDBODY_H_

#include "BrokenCore.h"
#include "Component.h"

#include "Math.h"
#include "PhysX_3.4/Include/PxPhysicsAPI.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ComponentDynamicRigidBody : public Component
{
public:
	ComponentDynamicRigidBody(GameObject* ContainerGO);
	virtual ~ComponentDynamicRigidBody();

	void Update() override;

	inline physx::PxReal GetMass() { return rigidBody->getMass(); }
	inline void SetMass(physx::PxReal mass) { rigidBody->setMass(mass); }
	inline void SetDensity(physx::PxReal density) { physx::PxRigidBodyExt::updateMassAndInertia(*rigidBody, density); }

	inline void UseGravity(bool enable) { rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !enable); }
	inline void SetKinematic(bool enable) { rigidBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, enable); }

	//Lineal
	inline float3 GetLinearVelocity() { return float3(rigidBody->getLinearVelocity().x, rigidBody->getLinearVelocity().y, rigidBody->getLinearVelocity().z); }
	inline float3 GetAngularVelocity() { return float3(rigidBody->getAngularVelocity().x, rigidBody->getAngularVelocity().y, rigidBody->getAngularVelocity().z); }
	inline void SetLinearVelocity(float3 vel) { rigidBody->setLinearVelocity(physx::PxVec3(vel.x, vel.y, vel.z)); }
	inline void SetAngularVelocity(float3 vel) { rigidBody->setAngularVelocity(physx::PxVec3(vel.x, vel.y, vel.z)); }

	inline void SetAngularDamping(physx::PxReal value) { rigidBody->setAngularDamping(value); }
	inline void SetLinearDamping(physx::PxReal value) { rigidBody->setLinearDamping(value); }
	inline physx::PxReal GetAngularDamping() { return rigidBody->getAngularDamping(); }
	inline physx::PxReal GetLinearDamping() { return rigidBody->getLinearDamping(); }

	//Acceleration
	inline void AddForce(physx::PxVec3 force, physx::PxForceMode::Enum mode) { rigidBody->addForce(force, mode); }
	inline void AddTorque(physx::PxVec3 force, physx::PxForceMode::Enum mode) { rigidBody->addTorque(force, mode); }

	inline void FeezePosition_X(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, enable); }
	inline void FeezePosition_Y(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, enable); }
	inline void FeezePosition_Z(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, enable); }
	inline void FreezeRotation_X(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, enable); }
	inline void FreezeRotation_Y(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, enable); }
	inline void FreezeRotation_Z(bool enable) { rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, enable); }

	static inline Component::ComponentType GetType() { return Component::ComponentType::DynamicRigidBody; };
	void UpdateRBValues();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

private:
	void StaticToDynamicRigidBody();

	void setRBValues();


public:
	physx::PxRigidDynamic* rigidBody = nullptr;

public:
	float mass = 10.f;
	float density = 1.f;
	bool use_gravity = true;
	bool is_kinematic = false;;
	float3 linear_vel = float3::zero;
	float3 angular_vel = float3::zero;
	float linear_damping = 0.f;
	float angular_damping = 0.f;
	bool freezePosition_X = false, freezePosition_Y = false, freezePosition_Z = false;
	bool freezeRotation_X = true, freezeRotation_Y = false, freezeRotation_Z = true;
	bool toPlay = false;
	bool update = false;
};

BE_END_NAMESPACE
#endif
