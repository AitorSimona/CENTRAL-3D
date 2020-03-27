#ifndef __PHYSX_SIMULATION_EVENTS_
#define __PHYSX_SIMULATION_EVENTS_

#include "BrokenCore.h"
#include "PhysX_3.4/Include/PxPhysicsAPI.h"

BE_BEGIN_NAMESPACE

class ModulePhysics;

class BROKEN_API PhysxSimulationEvents : public physx::PxSimulationEventCallback
{
public:
	PhysxSimulationEvents(ModulePhysics* callback);
	~PhysxSimulationEvents() {}

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
	
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) {}

	// Happen after fetchResults() (swaps the buffers)
	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}

private:

	ModulePhysics* callback = nullptr;
};

BE_END_NAMESPACE
#endif __PHYSX_SIMULATION_EVENTS_