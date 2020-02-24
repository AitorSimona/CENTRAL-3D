#include "ModuleParticles.h"
#include "ModulePhysics.h"
#include "Application.h"

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

ModuleParticles::ModuleParticles(bool start_enabled)
{
	name = "Particles Module";
}

ModuleParticles::~ModuleParticles()
{}

bool ModuleParticles::Init(json config)
{
	return true;
}

bool ModuleParticles::Start()
{
	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);

	if (particleSystem)
		App->physics->mScene->addActor(*particleSystem);

	PxParticleCreationData particleCreationData;

	static PxVec3 paco(3, 4, 5);
	static PxVec3 position(0, 0, 0);


	particleCreationData.numParticles = 10;
	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(PxU32(0));
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(&position);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(&paco);
	
	bool success = particleSystem->createParticles(particleCreationData);

	return true;
}

update_status ModuleParticles::Update(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
	particleSystem->releaseParticles();

	return true;
}
