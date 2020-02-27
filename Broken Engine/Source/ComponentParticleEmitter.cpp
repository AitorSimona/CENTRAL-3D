#include "ComponentParticleEmitter.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleTimeManager.h"
#include "Imgui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleParticles.h"
#include "Particle.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"


ComponentParticleEmitter::ComponentParticleEmitter(GameObject* ContainerGO):Component(ContainerGO, Component::ComponentType::ParticleEmitter)
{
	size = { 0,0,0 };
	emisionRate = 500.0f;

	particlesVelocity = { 10,0,0 };
	particlesDuration = 0;	

	Enable();
	
	App->particles->AddEmitter(this);
	
	particles.resize(maxParticles);
	
	for (int i= 0; i < maxParticles; ++i)
		particles[i] = new Particle();

	int i = 0;

}

ComponentParticleEmitter::~ComponentParticleEmitter()
{
	// MYTODO: Tell module particles to erase its pointer!!!
	App->particles->DeleteEmitter(this);
}

void ComponentParticleEmitter::Enable()
{
	active = true;

	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);

	if (particleSystem)
		App->physics->mScene->addActor(*particleSystem);

	indexPool = PxParticleExt::createIndexPool(maxParticles);
}

void ComponentParticleEmitter::Disable()
{
	particleSystem->releaseParticles();
	
	App->physics->mScene->removeActor(*particleSystem);
	
	indexPool->release();

	active = false;
}

void ComponentParticleEmitter::UpdateParticles(float dt)
{
	//Create particles
	if (SDL_GetTicks() - spawnClock > emisionRate)
	{
		if (validParticles < maxParticles)
		{
			validParticles++;
			spawnClock = SDL_GetTicks();

			PxParticleCreationData creationData;

			//Create 1 particle each time
			creationData.numParticles = 1;
			PxU32 index[1];

			const PxStrideIterator<PxU32> indexBuffer(index);

			indexPool->allocateIndices(1, indexBuffer);

			float3 globalPosition = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();

			PxVec3 positionBuffer[] ={ PxVec3( globalPosition.x, globalPosition.y, globalPosition.z) };
			PxVec3 velocityBuffer[] = { particlesVelocity };

			creationData.indexBuffer = indexBuffer;
			creationData.positionBuffer = PxStrideIterator<const PxVec3>(positionBuffer);
			creationData.velocityBuffer = PxStrideIterator<const PxVec3>(velocityBuffer);

			bool succes = particleSystem->createParticles(creationData);
		}
	}

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
				float3 newPosition(positionIt->x, positionIt->y, positionIt->z);
				particles[i]->position =newPosition;
			}
		}

		// return ownership of the buffers back to the SDK
		rd->unlock();
	}
}

void ComponentParticleEmitter::DrawParticles()
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->Draw();
}

json ComponentParticleEmitter::Save() const
{
	return json();
}

void ComponentParticleEmitter::Load(json& node)
{
}

void ComponentParticleEmitter::CreateInspectorNode()
{
	//Emitter size

	ImGui::Text("Emitter size");

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("X", &size.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("Y", &size.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("Z", &size.z, 0.005f);

	//Emision rate
	ImGui::DragFloat("Emision rate (ms) ", &emisionRate, 5.0f,10.00f ,1000000.0f);

	//External forces
	ImGui::Text("External forces ");
	PxVec3 externalForce(0, 0, 0);
	if (ImGui::SliderFloat("##X", &externalForce.x, -10000.0f, 10000.0f) ||
		ImGui::SliderFloat("##y", &externalForce.y, -10000.0f, 10000.0f) ||
		ImGui::SliderFloat("##z", &externalForce.z, -10000.0f, 10000.0f))
		particleSystem->setExternalAcceleration(externalForce);

}
