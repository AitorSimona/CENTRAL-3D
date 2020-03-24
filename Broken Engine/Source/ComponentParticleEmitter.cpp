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
#include "RandomGenerator.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"


using namespace Broken;

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* ContainerGO):Component(ContainerGO, Component::ComponentType::ParticleEmitter)
{
	Enable();

	App->particles->AddEmitter(this);

	particles.resize(maxParticles);

	for (int i= 0; i < maxParticles; ++i)
		particles[i] = new Particle();
}

ComponentParticleEmitter::~ComponentParticleEmitter()
{
	App->particles->DeleteEmitter(this);

	for (int i = 0; i < maxParticles; ++i){
		delete particles[i];
	}

	if (particleSystem && App->physics->mScene) {
		particleSystem->releaseParticles();
		App->physics->mScene->removeActor(*particleSystem);
		indexPool->release();
		particles.clear();
	}
}

void ComponentParticleEmitter::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentParticleEmitter::Enable()
{
	active = true;

	particleSystem = App->physics->mPhysics->createParticleSystem(maxParticles, perParticleRestOffset);
	particleSystem->setMaxMotionDistance(100);

	if (particleSystem)
		App->physics->mScene->addActor(*particleSystem);

	indexPool = physx::PxParticleExt::createIndexPool(maxParticles);

	particleSystem->setExternalAcceleration(externalAcceleration);
}

void ComponentParticleEmitter::Disable()
{

	App->physics->mScene->removeActor(*particleSystem);

	active = false;
}

void ComponentParticleEmitter::UpdateParticles(float dt)
{
	//Create particle depending on the time
	if (loop || emisionActive) {
		if (SDL_GetTicks() - spawnClock > emisionRate)
		{
			uint newParticlesAmount = (SDL_GetTicks() - spawnClock) / emisionRate;

			CreateParticles(newParticlesAmount);

			//if (validParticles < maxParticles)
			//{
			//	if (newParticlesAmount > maxParticles - validParticles)
			//		newParticlesAmount = maxParticles - validParticles;
			//	validParticles += newParticlesAmount;
			//	spawnClock = SDL_GetTicks();
			//	physx::PxParticleCreationData creationData;
			//	//Create 1 particle each time
			//	creationData.numParticles = newParticlesAmount;
			//	physx::PxU32* index = new physx::PxU32[newParticlesAmount];
			//	const physx::PxStrideIterator<physx::PxU32> indexBuffer(index);
			//	indexPool->allocateIndices(newParticlesAmount, indexBuffer);
			//	float3 globalPosition = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();
			//	physx::PxVec3* positionBuffer = new physx::PxVec3[newParticlesAmount];
			//	physx::PxVec3* velocityBuffer = new physx::PxVec3[newParticlesAmount];
			//	for (int i = 0; i < newParticlesAmount; ++i) {
			//	velocityBuffer[i] = { physx::PxVec3(particlesVelocity.x + GetRandomValue(-velocityRandomFactor.x, velocityRandomFactor.x),
			//									particlesVelocity.y + GetRandomValue(-velocityRandomFactor.y,velocityRandomFactor.y),
			//									particlesVelocity.z + GetRandomValue(-velocityRandomFactor.z,velocityRandomFactor.z)) };
			//	positionBuffer[i] = { physx::PxVec3(globalPosition.x + GetRandomValue(-size.x,size.x),
			//									globalPosition.y + GetRandomValue(-size.y,size.y),
			//									globalPosition.z + GetRandomValue(-size.z,size.z)) };
			//	particles[index[i]]->lifeTime = particlesLifeTime;
			//	particles[index[i]]->spawnTime = SDL_GetTicks();
			//	particles[index[i]]->color = particlesColor/255.0f;
			//	}
			//	creationData.indexBuffer = indexBuffer;
			//	creationData.positionBuffer = physx::PxStrideIterator<const physx::PxVec3>(positionBuffer);
			//	creationData.velocityBuffer = physx::PxStrideIterator<const physx::PxVec3>(velocityBuffer);
			//	bool succes = particleSystem->createParticles(creationData);
			//	delete[] index;
			//	delete[] positionBuffer;
			//	delete[] velocityBuffer;
			//}
		}

		if (emisionActive)
		{
			if (SDL_GetTicks() - emisionStart > duration)
				emisionActive = false;
		}
	}
		//Update particles
		//lock SDK buffers of *PxParticleSystem* ps for reading
		physx::PxParticleReadData* rd = particleSystem->lockParticleReadData();

		std::vector<physx::PxU32> indicesToErease;
		uint particlesToRelease = 0;

		// access particle data from physx::PxParticleReadData
		if (rd)
		{
			physx::PxStrideIterator<const physx::PxParticleFlags> flagsIt(rd->flagsBuffer);
			physx::PxStrideIterator<const physx::PxVec3> positionIt(rd->positionBuffer);

			for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionIt)
			{
				if (*flagsIt & physx::PxParticleFlag::eVALID)
				{
					//Check if particle should die
					if (SDL_GetTicks() - particles[i]->spawnTime > particles[i]->lifeTime) {
						indicesToErease.push_back(i);
						particlesToRelease++;
						continue;
					}

					//Update particle position
					float3 newPosition(positionIt->x, positionIt->y, positionIt->z);
					particles[i]->position = newPosition;
					particles[i]->diameter = particlesSize;
				}
			}
			// return ownership of the buffers back to the SDK
			rd->unlock();
		}
		if (particlesToRelease > 0) {

			particleSystem->releaseParticles(particlesToRelease, physx::PxStrideIterator<physx::PxU32>(indicesToErease.data()));
			validParticles -= particlesToRelease;
			indexPool->freeIndices(particlesToRelease, physx::PxStrideIterator<physx::PxU32>(indicesToErease.data()));
		}
	
}

void ComponentParticleEmitter::DrawParticles()
{
	physx::PxParticleReadData* rd = particleSystem->lockParticleReadData();
	if (rd)
	{
		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIt(rd->flagsBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt)
		{
			if (*flagsIt & physx::PxParticleFlag::eVALID)
			{
				particles[i]->Draw();
			}
		}

		// return ownership of the buffers back to the SDK
		rd->unlock();
	}
}


void ComponentParticleEmitter::ChangeParticlesColor(float3 color)
{
	color /= 255.0f;

	for (int i = 0; i < maxParticles; ++i)
		particles[i]->color = color;
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

	node["ColorR"] = std::to_string(particlesColor.x);
	node["ColorG"] = std::to_string(particlesColor.y);
	node["ColorB"] = std::to_string(particlesColor.z);

	return node;
}

void ComponentParticleEmitter::Load(json& node)
{
	//load the strings
	std::string Lsizex = node["sizeX"].is_null() ? "0" : node["sizeX"];
	std::string Lsizey = node["sizeY"].is_null() ? "0" : node["sizeY"];
	std::string Lsizez = node["sizeZ"].is_null() ? "0" : node["sizeZ"];

	std::string LemisionRate = node["emisionRate"].is_null() ? "0" : node["emisionRate"]; // typo: emission

	std::string LexternalAccelerationx = node["externalAccelerationX"].is_null() ? "0" : node["externalAccelerationX"];
	std::string LexternalAccelerationy = node["externalAccelerationY"].is_null() ? "0" : node["externalAccelerationY"];
	std::string LexternalAccelerationz = node["externalAccelerationZ"].is_null() ? "0" : node["externalAccelerationZ"];

	std::string LparticlesVelocityx = node["particlesVelocityX"].is_null() ? "0" : node["particlesVelocityX"];
	std::string LparticlesVelocityy = node["particlesVelocityY"].is_null() ? "0" : node["particlesVelocityY"];
	std::string LparticlesVelocityz	= node["particlesVelocityZ"].is_null() ? "0" : node["particlesVelocityZ"];

	std::string LvelocityRandomFactorx = node["velocityRandomFactorX"].is_null() ? "0" : node["velocityRandomFactorX"];
	std::string LvelocityRandomFactory = node["velocityRandomFactorY"].is_null() ? "0" : node["velocityRandomFactorY"];
	std::string LvelocityRandomFactorz = node["velocityRandomFactorZ"].is_null() ? "0" : node["velocityRandomFactorZ"];

	std::string LparticlesLifeTime = node["particlesLifeTime"].is_null() ? "0" : node["particlesLifeTime"];

	std::string LParticlesSize = node["particlesSize"].is_null() ? "0" : node["particlesSize"];

	std::string LColorR = node["ColorR"].is_null() ? "0" : node["ColorR"];
	std::string LColorG = node["ColorG"].is_null() ? "0" : node["ColorG"];
	std::string LColorB = node["ColorB"].is_null() ? "0" : node["ColorB"];

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

	particlesColor = float3(std::stof(LColorR), std::stof(LColorG), std::stof(LColorB));
}

void ComponentParticleEmitter::CreateInspectorNode()
{
	if (ImGui::Checkbox("Active", &active)) {
		if (active)
			Enable();
		else
			Disable();
	}

	ImGui::Text("Loop");
	ImGui::SameLine();
	ImGui::Checkbox("##PELoop", &loop);


	ImGui::Text("Duration");
	ImGui::SameLine();
	if (ImGui::DragInt("##PEDuration", &duration))
		Play();

	if (ImGui::Button("Delete component"))
		to_delete = true;

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

	//Particles Color particlesColor
	ImGui::Text("Particles Color");
	bool colorChanged = false;

	//R
	ImGui::Text("R");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if(ImGui::DragFloat("##ColorR", &particlesColor.x, 0.05f, 0.0f, 255.0f))
		colorChanged = true;

	ImGui::SameLine();
	//G
	ImGui::Text("G");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if(ImGui::DragFloat("##ColorG", &particlesColor.y, 0.05f, 0.0f, 255.0f))
		colorChanged = true;

	//B
	ImGui::SameLine();
	ImGui::Text("B");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	if (ImGui::DragFloat("##ColorB", &particlesColor.z, 0.05f, 0.0f, 255.0f))
		colorChanged = true;

	/*if (colorChanged)
		ChangeParticlesColor(particlesColor);*/
}

double ComponentParticleEmitter::GetRandomValue(double min,double max) //EREASE IN THE FUTURE
{
	return App->RandomNumberGenerator.GetDoubleRNinRange(min, max);
}

void ComponentParticleEmitter::CreateParticles(uint particlesAmount)
{
	uint particlesToCreate = particlesAmount;

	if (validParticles < maxParticles)
	{
		Quat rotation= GO->GetComponent<ComponentTransform>()->rotation;

		if (particlesToCreate > maxParticles - validParticles)
			particlesToCreate = maxParticles - validParticles;

		validParticles += particlesToCreate;
		spawnClock = SDL_GetTicks();

		physx::PxParticleCreationData creationData;

		//Create 1 particle each time
		creationData.numParticles = particlesToCreate;
		physx::PxU32* index = new physx::PxU32[particlesToCreate];

		const physx::PxStrideIterator<physx::PxU32> indexBuffer(index);

		indexPool->allocateIndices(particlesToCreate, indexBuffer);

		float3 globalPosition = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();

		physx::PxVec3* positionBuffer = new physx::PxVec3[particlesToCreate];
		physx::PxVec3* velocityBuffer = new physx::PxVec3[particlesToCreate];

		for (int i = 0; i < particlesToCreate; ++i) {
			velocityBuffer[i] = { physx::PxVec3((particlesVelocity.x + GetRandomValue(-velocityRandomFactor.x, velocityRandomFactor.x)),
											particlesVelocity.y + GetRandomValue(-velocityRandomFactor.y,velocityRandomFactor.y),
											particlesVelocity.z + GetRandomValue(-velocityRandomFactor.z,velocityRandomFactor.z)) };


			Quat quat = Quat(velocityBuffer[i].x, velocityBuffer[i].y, velocityBuffer[i].z,0);

			quat = rotation * quat * rotation.Conjugated();

			velocityBuffer[i] = physx::PxVec3(quat.x, quat.y, quat.z);

			positionBuffer[i] = { physx::PxVec3(globalPosition.x + GetRandomValue(-size.x,size.x),
											globalPosition.y + GetRandomValue(-size.y,size.y),
											globalPosition.z + GetRandomValue(-size.z,size.z)) };

			particles[index[i]]->lifeTime = particlesLifeTime;
			particles[index[i]]->spawnTime = SDL_GetTicks();
			particles[index[i]]->color = particlesColor / 255.0f;
		}

		creationData.indexBuffer = indexBuffer;
		creationData.positionBuffer = physx::PxStrideIterator<const physx::PxVec3>(positionBuffer);
		creationData.velocityBuffer = physx::PxStrideIterator<const physx::PxVec3>(velocityBuffer);

		bool succes = particleSystem->createParticles(creationData);

		delete[] index;
		delete[] positionBuffer;
		delete[] velocityBuffer;
	}
}

void ComponentParticleEmitter::Play()
{
	emisionActive = true;
	emisionStart = SDL_GetTicks();
}