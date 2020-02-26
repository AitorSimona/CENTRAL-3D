#include "ComponentParticleEmitter.h"
#include "Application.h"
#include "ModulePhysics.h"


#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#ifndef _DEBUG
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxFoundationCHECKED_x86.lib")
#else
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxFoundationDEBUG_x86.lib")
#endif // _DEBUG

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* ContainerGO):Component(ContainerGO, Component::ComponentType::ParticleEmitter)
{}

ComponentParticleEmitter::~ComponentParticleEmitter()
{}

void ComponentParticleEmitter::Enable()
{
	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);

	//Delete gravity
	particleSystem->setExternalAcceleration(PxVec3(0, 9.80, 0));

	if (particleSystem)
		App->physics->mScene->addActor(*particleSystem);

	indexPool = PxParticleExt::createIndexPool(maxParticles);
}

void ComponentParticleEmitter::Disable()
{
	particleSystem->releaseParticles();
	indexPool->release();
}

void ComponentParticleEmitter::UpdateParticles(float dt)
{


}

void ComponentParticleEmitter::CreateInspectorNode()
{

}
