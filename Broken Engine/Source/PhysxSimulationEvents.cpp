#include "PhysxSimulationEvents.h"
#include "ModulePhysics.h"

using namespace Broken;

PhysxSimulationEvents::PhysxSimulationEvents(ModulePhysics* callback)
{
	this->callback = callback;
}

void PhysxSimulationEvents::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
 	int a = 0;
}

void PhysxSimulationEvents::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	collider;
}

