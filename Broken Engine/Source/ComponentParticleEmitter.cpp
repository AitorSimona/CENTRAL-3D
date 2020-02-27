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
	particlesVelocity = { 10,10,10 };

	Enable();
	
	App->particles->AddEmitter(this);
	
	particles.resize(maxParticles);
	
	for (int i= 0; i < maxParticles; ++i)
		particles[i] = new Particle();
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

	particleSystem->setExternalAcceleration(externalAcceleration);
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
	//Create particle if emision rate allows it
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

			particles[index[0]]->lifeTime = particlesLifeTime;
			particles[index[0]]->spawnTime = SDL_GetTicks();
		}
	}


	//Update particles
	// lock SDK buffers of *PxParticleSystem* ps for reading
	PxParticleReadData* rd = particleSystem->lockParticleReadData();

	std::vector<PxU32> indicesToErease;
	uint particlesToRelease = 0;
	
	// access particle data from PxParticleReadData
	if (rd)
	{
		PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionIt)
		{
			if (*flagsIt & PxParticleFlag::eVALID)
			{
				//Check if particle should die
				if (SDL_GetTicks() - particles[i]->spawnTime > particles[i]->lifeTime) {
					indicesToErease.push_back(i);
					particlesToRelease++;
					continue;
				}

				//Update particle position
				float3 newPosition(positionIt->x, positionIt->y, positionIt->z);
				particles[i]->position =newPosition;
			}
		}

		
		// return ownership of the buffers back to the SDK
		rd->unlock();
	}

		if (particlesToRelease > 0) 
			particleSystem->releaseParticles(particlesToRelease, PxStrideIterator<PxU32>(indicesToErease.data()));
}

void ComponentParticleEmitter::DrawParticles()
{
	PxParticleReadData* rd = particleSystem->lockParticleReadData();
	if (rd)
	{
		PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt)
		{
			if (*flagsIt & PxParticleFlag::eVALID)
			{
				particles[i]->Draw();
			}
		}

		// return ownership of the buffers back to the SDK
		rd->unlock();
	}
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

	ImGui::DragFloat("##SEmitterX", &size.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SEmitterY", &size.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SEmitterZ", &size.z, 0.005f);

	//Emision rate
	ImGui::Text("Emision rate (ms)");
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
	ImGui::DragFloat("##SEmision rate", &emisionRate, 5.0f,10.00f ,1000000.0f);

	//External forces
	ImGui::Text("External forces ");
	bool forceChanged = false;
	//X
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SX", &externalAcceleration.x, 0.005f,-10.0f,10.0f))
		forceChanged = true;

	ImGui::SameLine();
	//Y
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SY", &externalAcceleration.y, 0.005f, -10.0f, 10.0f))
		forceChanged = true;
	//Z
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SZ", &externalAcceleration.z, 0.005f, -10.0f, 10.0f))
		forceChanged = true;

	if (forceChanged)
		particleSystem->setExternalAcceleration(externalAcceleration);

	//Particles velocity
	ImGui::Text("Particles velocity");
	//X
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SVelocityX", &particlesVelocity.x, 0.05f, -100.0f, 100.0f);

	ImGui::SameLine();
	//Y
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SVelocityY", &particlesVelocity.y, 0.05f, -100.0f, 100.0f);
	//Z
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SVelocityZ", &particlesVelocity.z, 0.05f, -100.0f, 100.0f);

	//Particles lifetime
	ImGui::DragInt("Particles lifetime (ms)", &particlesLifeTime, 50.0f);

}
