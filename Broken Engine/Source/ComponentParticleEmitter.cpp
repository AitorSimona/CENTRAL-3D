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
	Enable();
	
	App->particles->AddEmitter(this);
	
	particles.resize(maxParticles);
	
	for (int i= 0; i < maxParticles; ++i)
		particles[i] = new Particle();

	m_RNEngine = std::default_random_engine(m_RandomDevice());
}

ComponentParticleEmitter::~ComponentParticleEmitter()
{
	App->particles->DeleteEmitter(this);

	if (particleSystem)
		App->physics->mScene->removeActor(*particleSystem);


	for (int i = 0; i < maxParticles; ++i){
		delete particles[i];
	}

	indexPool->release();
	particles.clear();
}

void ComponentParticleEmitter::Enable()
{
	active = true;

	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);
	particleSystem->setMaxMotionDistance(1000);

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

			PxVec3 positionBuffer[] ={ PxVec3(	globalPosition.x + GetRandomValue(-size.x,size.x),
												globalPosition.y + GetRandomValue(-size.y,size.y),
												globalPosition.z + GetRandomValue(-size.z,size.z))};

			PxVec3 velocityBuffer[] = { PxVec3(	particlesVelocity.x + GetRandomValue(-velocityRandomFactor.x,velocityRandomFactor.x) ,
												particlesVelocity.y + GetRandomValue(-velocityRandomFactor.y,velocityRandomFactor.y),
												particlesVelocity.z + GetRandomValue(-velocityRandomFactor.z,velocityRandomFactor.z)) };



			creationData.indexBuffer = indexBuffer;
			creationData.positionBuffer = PxStrideIterator<const PxVec3>(positionBuffer);
			creationData.velocityBuffer = PxStrideIterator<const PxVec3>(velocityBuffer);

			bool succes = particleSystem->createParticles(creationData);

			particles[index[0]]->lifeTime = particlesLifeTime;
			particles[index[0]]->spawnTime = SDL_GetTicks();
		}
	}

	//Update particles
	//lock SDK buffers of *PxParticleSystem* ps for reading
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
	if (particlesToRelease > 0) {

		particleSystem->releaseParticles(particlesToRelease, PxStrideIterator<PxU32>(indicesToErease.data()));
		validParticles -= particlesToRelease;
		indexPool->freeIndices(particlesToRelease, PxStrideIterator<PxU32>(indicesToErease.data()));
	}
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
	json node;

	node["sizeX"]=std::to_string(size.x);
	node["sizeY"] = std::to_string(size.y);
	node["sizeZ"] = std::to_string(size.z);

	node["emisionRate"] = std::to_string(emisionRate);

	node["externalAccelerationX"] = std::to_string(externalAcceleration.x);
	node["externalAccelerationY"] = std::to_string(externalAcceleration.y);
	node["externalAccelerationZ"] = std::to_string(externalAcceleration.z);

	node["particlesVelocityX"] = std::to_string(particlesVelocity.x);
	node["particlesVelocityY"] = std::to_string(particlesVelocity.y);
	node["particlesVelocityZ"] = std::to_string(particlesVelocity.z);

	node["velocityRandomFactorX"] = std::to_string(velocityRandomFactor.x);
	node["velocityRandomFactorY"] = std::to_string(velocityRandomFactor.y);
	node["velocityRandomFactorZ"] = std::to_string(velocityRandomFactor.z);

	node["particlesLifeTime"] = std::to_string(particlesLifeTime);

	return node;
}

void ComponentParticleEmitter::Load(json& node)
{
	size.x = node["sizeX"];
	size.y = node["sizeY"];
	size.z = node["sizeZ"];

	emisionRate = node["emisionRate"];

	externalAcceleration.x = node["externalAccelerationX"];
	externalAcceleration.y = node["externalAccelerationY"];
	externalAcceleration.z = node["externalAccelerationZ"];

	particlesVelocity.x = node["particlesVelocityX"];
	particlesVelocity.y = node["particlesVelocityY"];
	particlesVelocity.z	= node["particlesVelocityZ"];

	velocityRandomFactor.x = node["velocityRandomFactorX"];
	velocityRandomFactor.y = node["velocityRandomFactorY"];
	velocityRandomFactor.z = node["velocityRandomFactorZ"];

	particlesLifeTime = node["particlesLifeTime"];
}

void ComponentParticleEmitter::CreateInspectorNode()
{
	//Emitter size

	ImGui::Text("Emitter size");

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SEmitterX", &size.x, 0.05f, 0.0f, 100.0f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SEmitterY", &size.y, 0.05f,0.0f, 100.0f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SEmitterZ", &size.z, 0.05f, 0.0f, 100.0f);

	//Emision rate
	ImGui::Text("Emision rate (ms)");
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
	ImGui::DragFloat("##SEmision rate", &emisionRate, 5.0f, 1.0f ,100000.0f);

	//External forces
	ImGui::Text("External forces ");
	bool forceChanged = false;
	//X
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesX", &externalAcceleration.x, 0.005f,-50.0f,50.0f))
		forceChanged = true;

	ImGui::SameLine();
	//Y
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesY", &externalAcceleration.y, 0.005f, -50.0f, 50.0f))
		forceChanged = true;
	//Z
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesZ", &externalAcceleration.z, 0.005f, -50.0f, 50.0f))
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

	//Random velocity factor
	ImGui::Text("Velocity random factor");
	//X
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SRandomVelocityX", &velocityRandomFactor.x, 0.05f, 0.0f, 100.0f);

	ImGui::SameLine();
	//Y
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SRandomVelocityY", &velocityRandomFactor.y, 0.05f, 0.0f, 100.0f);
	//Z
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##SRandomVelocityZ", &velocityRandomFactor.z, 0.05f, 0.0f, 100.0f);

	//Particles lifetime
	ImGui::Text("Particles lifetime (ms)");
	ImGui::DragInt("##SParticlesLifetime", &particlesLifeTime, 50.0f,0.0f, 10000.0f);

}

double ComponentParticleEmitter::GetRandomValue(double min,double max) //EREASE IN THE FUTURE
{
	std::uniform_real_distribution<double> tmp_DoubleDistribution(min, max);
	return tmp_DoubleDistribution(m_RNEngine);
}