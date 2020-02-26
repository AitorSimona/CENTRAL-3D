#include "ModuleParticles.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleInput.h"

#include "ComponentParticleEmitter.h"

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

	particleSystem->setExternalAcceleration(PxVec3(0, -9.80, 0));

	if (particleSystem)
		App->physics->mScene->addActor(*particleSystem);

	//TEST
	//Creating particles
	PxParticleCreationData creationData;

	creationData.numParticles = 3;

	PxU32 indexBuffer[] = { 0,1,2 };
	PxVec3 positionBuffer[] = { PxVec3(-10,30,0),PxVec3(0,30,0),PxVec3(14,30,0) };
	PxVec3 velocityBuffer[] = { PxVec3(0,0,0),PxVec3(0,0,0) ,PxVec3(0,0,0) };

	creationData.indexBuffer = PxStrideIterator<const PxU32>(indexBuffer);
	creationData.positionBuffer = PxStrideIterator<const PxVec3>(positionBuffer);
	creationData.velocityBuffer = PxStrideIterator<const PxVec3>(velocityBuffer);

	bool succes = particleSystem->createParticles(creationData);

	//TEST

	return true;
}

update_status ModuleParticles::Update(float dt)
{

	for (int i = 0; i < particleEmitters.size(); ++i)
		particleEmitters[i]->UpdateParticles(dt);


	//TEST

	// lock SDK buffers of *PxParticleSystem* ps for reading
	PxParticleReadData* rd = particleSystem->lockParticleReadData();

	// access particle data from PxParticleReadData
	if (rd)
	{
		PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionIt)
		{
			if (*flagsIt & PxParticleFlag::eVALID)
			{
				// access particle position
				const PxVec3& position = *positionIt;
				EngineConsoleLog("%f", position.x,"%f", position.y, "%f", position.z, "%f");
			}
		}

		// return ownership of the buffers back to the SDK
		rd->unlock();
	}

	//PxU32 numParticles = 1;
	//PxU32 indices[] = { 0 };
	//PxVec3 positions[] = { PxVec3(10,9.8,30) };

	//if (App->input->GetKey(SDL_SCANCODE_SPACE)==KEY_DOWN)
	//	particleSystem->setPositions(numParticles,PxStrideIterator<const PxU32>(indices), PxStrideIterator<const PxVec3>(positions));

	//if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
	//	particleSystem->addForces(numParticles, PxStrideIterator<const PxU32>(indices), PxStrideIterator<const PxVec3>(positions), PxForceMode::eACCELERATION);

	//if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	//	particleSystem->setVelocities(numParticles, PxStrideIterator<const PxU32>(indices), PxStrideIterator<const PxVec3>(positions));
	//
	////TEST

	return UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
	particleSystem->releaseParticles();

	return true;
}
