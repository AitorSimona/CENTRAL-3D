#include "ComponentParticleEmitter.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleTimeManager.h"
#include "Imgui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleParticles.h"
#include "Particle.h"
#include "Timer.h"

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

	for (int i = 0; i < maxParticles; ++i){
		delete particles[i];
		//particles[i] = nullptr;
	}

	if (particleSystem && App->physics->mScene) {
		App->physics->mScene->removeActor(*particleSystem);
		indexPool->release();
		particles.clear();
	}
}

void ComponentParticleEmitter::Enable()
{
	active = true;

	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);
	particleSystem->setMaxMotionDistance(100);

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
				particles[i]->diameter = particlesSize;
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

	node["particlesSize"] = std::to_string(particlesSize);

	return node;
}

void ComponentParticleEmitter::Load(json& node)
{
	//load the strings
	std::string Lsizex = node["sizeX"];
	std::string Lsizey = node["sizeY"];
	std::string Lsizez = node["sizeZ"];

	std::string LemisionRate = node["emisionRate"];

	std::string LexternalAccelerationx = node["externalAccelerationX"];
	std::string LexternalAccelerationy = node["externalAccelerationY"];
	std::string LexternalAccelerationz = node["externalAccelerationZ"];

	std::string LparticlesVelocityx = node["particlesVelocityX"];
	std::string LparticlesVelocityy = node["particlesVelocityY"];
	std::string LparticlesVelocityz	= node["particlesVelocityZ"];

	std::string LvelocityRandomFactorx = node["velocityRandomFactorX"];
	std::string LvelocityRandomFactory = node["velocityRandomFactorY"];
	std::string LvelocityRandomFactorz = node["velocityRandomFactorZ"];

	std::string LparticlesLifeTime = node["particlesLifeTime"];

	std::string LParticlesSize = node["particlesSize"];

	//Pass the strings to the needed dada types
	size.x = std::stof(Lsizex);
	size.y = std::stof(Lsizey);
	size.z = std::stof(Lsizez);

	emisionRate = std::stof(LemisionRate);

	externalAcceleration.x = std::stof(LexternalAccelerationx);
	externalAcceleration.y = std::stof(LexternalAccelerationy);
	externalAcceleration.z = std::stof(LexternalAccelerationz);

	particlesVelocity.x = std::stof(LparticlesVelocityx);
	particlesVelocity.y = std::stof(LparticlesVelocityy);
	particlesVelocity.z = std::stof(LparticlesVelocityz);

	velocityRandomFactor.x = std::stof(LvelocityRandomFactorx);
	velocityRandomFactor.y = std::stof(LvelocityRandomFactory);
	velocityRandomFactor.z = std::stof(LvelocityRandomFactorz);

	particlesLifeTime = std::stof(LparticlesLifeTime);

	particlesSize = std::stof(LParticlesSize);
}

void ComponentParticleEmitter::CreateInspectorNode()
{
	if (ImGui::Checkbox("Active", &active)) {
		if (active)
			Enable();
		else
			Disable();
	}

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
	ImGui::DragFloat("##SEmision rate", &emisionRate, 1.0f, 1.0f, 100000.0f);

	//External forces
	ImGui::Text("External forces ");
	bool forceChanged = false;
	//X
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesX", &externalAcceleration.x, 0.05f,-50.0f,50.0f))
		forceChanged = true;

	ImGui::SameLine();
	//Y
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesY", &externalAcceleration.y, 0.05f, -50.0f, 50.0f))
		forceChanged = true;
	//Z
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##SExternalforcesZ", &externalAcceleration.z, 0.05f, -50.0f, 50.0f))
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
	ImGui::DragInt("##SParticlesLifetime", &particlesLifeTime, 3.0f, 0.0f, 10000.0f);

	//Particles size
	ImGui::Text("Particles size");
	ImGui::DragFloat("##SParticlesSize", &particlesSize, 0.005f, 0.01f, 3.0f);

}

double ComponentParticleEmitter::GetRandomValue(double min,double max) //EREASE IN THE FUTURE
{
	std::uniform_real_distribution<double> tmp_DoubleDistribution(min, max);
	return tmp_DoubleDistribution(m_RNEngine);
}