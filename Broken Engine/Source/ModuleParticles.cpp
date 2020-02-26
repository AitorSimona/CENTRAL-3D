#include "ModuleParticles.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleInput.h"

#include "ComponentParticleEmitter.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"


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

	return true;
}

update_status ModuleParticles::Update(float dt)
{

	for (int i = 0; i < particleEmitters.size(); ++i)
		if (particleEmitters[i]->IsEnabled())
			particleEmitters[i]->UpdateParticles(dt);

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

void ModuleParticles::DrawParticles()
{
	for (uint i = 0; i < particleEmitters.size(); ++i)
		particleEmitters[i]->DrawParticles();
}

void ModuleParticles::AddEmitter(ComponentParticleEmitter* componentEmitter)
{
	particleEmitters.push_back(componentEmitter);
}

void ModuleParticles::DeleteEmitter(ComponentParticleEmitter* componentEmitter)
{
	for (std::vector<ComponentParticleEmitter*>::iterator it = particleEmitters.begin();it != particleEmitters.end() ; it++) {
		if (*(it) == componentEmitter) {
			particleEmitters.erase(it);
			break;
		}
	}
}

bool ModuleParticles::CleanUp()
{

	return true;
}
