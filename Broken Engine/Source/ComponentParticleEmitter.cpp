#include "Application.h"
#include "Imgui/imgui.h"
#include "GameObject.h"
#include "Timer.h"
#include "RandomGenerator.h"
#include "ResourceTexture.h"

#include "ComponentParticleEmitter.h"
#include "ComponentTransform.h"
#include "ComponentText.h"
#include "ComponentCamera.h"

#include "ModuleTimeManager.h"
#include "ModulePhysics.h"
#include "ModuleParticles.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"

#include "Particle.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"


using namespace Broken;

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* ContainerGO):Component(ContainerGO, Component::ComponentType::ParticleEmitter)
{
	name = "Particle Emitter";
	Enable();

	App->particles->AddEmitter(this);

	particles.resize(maxParticles);

	for (int i= 0; i < maxParticles; ++i)
		particles[i] = new Particle();

	texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "DefaultTexture");

	App->renderer3D->particleEmitters.push_back(this);
}

ComponentParticleEmitter::~ComponentParticleEmitter()
{
	drawingIndices.clear();

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

	texture->Release();


	for (std::vector<ComponentParticleEmitter*>::iterator it = App->renderer3D->particleEmitters.begin(); it != App->renderer3D->particleEmitters.end();it++)
		if ((*it) == this) {
			App->renderer3D->particleEmitters.erase(it);
			break;
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
	// Create particle depending on the time
	if (emisionActive && App->GetAppState() == AppState::PLAY) {
		if (App->time->GetGameplayTimePassed()*1000 - spawnClock > emisionRate)
		{
			uint newParticlesAmount = (App->time->GetGameplayTimePassed() * 1000 - spawnClock) / emisionRate;
			CreateParticles(newParticlesAmount*particlesPerCreation);			
		}

		if (emisionActive && !loop)
		{
			if ((App->time->GetGameplayTimePassed() * 1000) - emisionStart > duration)
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
				if (App->time->GetGameplayTimePassed()*1000 - particles[i]->spawnTime > particles[i]->lifeTime) {
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

	SortParticles();		
}

void ComponentParticleEmitter::SortParticles()
{
	physx::PxParticleReadData* rd = particleSystem->lockParticleReadData();
	if (rd)
	{
		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIt(rd->flagsBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt)
		{
			if (*flagsIt & physx::PxParticleFlag::eVALID)
			{
				float distance = App->renderer3D->active_camera->frustum.Pos().Distance(particles[i]->position);
				drawingIndices[1.0f/distance] = i;
			}
		}

		// return ownership of the buffers back to the SDK
		rd->unlock();
	}
}

void ComponentParticleEmitter::DrawParticles()
{
	if (!active || drawingIndices.empty())
		return;

	//while (!drawingIndices.empty())
	//{
	//	//particles[drawingIndices.front()]->Draw();

	//	drawingIndices.pop();
	//}

	//physx::PxParticleReadData* rd = particleSystem->lockParticleReadData();
	//if (rd)
	//{
	//	physx::PxStrideIterator<const physx::PxParticleFlags> flagsIt(rd->flagsBuffer);

	//	for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt)
	//	{
	//		if (*flagsIt & physx::PxParticleFlag::eVALID)
	//		{
	//			particles[i]->Draw();
	//		}
	//	}

	//	// return ownership of the buffers back to the SDK
	//	rd->unlock();
	//}

	std::map<float, int>::iterator it = drawingIndices.begin();
	while (it != drawingIndices.end())
	{
		int paco = (*it).second;
		particles[paco]->Draw();
		it++;
	}
	drawingIndices.clear();
}

//void ComponentParticleEmitter::DrawComponent()
//{
//	if (!active)
//		return;
//
//	physx::PxParticleReadData* rd = particleSystem->lockParticleReadData();
//	if (rd)
//	{
//		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIt(rd->flagsBuffer);
//
//		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt)
//		{
//			if (*flagsIt & physx::PxParticleFlag::eVALID)
//			{
//				particles[i]->Draw();
//			}
//		}
//
//		// return ownership of the buffers back to the SDK
//		rd->unlock();
//	}
//}

void ComponentParticleEmitter::ChangeParticlesColor(float3 color)
{
	color /= 255.0f;

	for (int i = 0; i < maxParticles; ++i)
		particles[i]->color = color;
}


json ComponentParticleEmitter::Save() const
{
	json node;

	node["Active"] = this->active;

	node["sizeX"]=std::to_string(size.x);
	node["sizeY"] = std::to_string(size.y);
	node["sizeZ"] = std::to_string(size.z);

	node["emisionRate"] = std::to_string(emisionRate);

	node["particlesPerCreation"] = std::to_string(particlesPerCreation);

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

	node["Loop"] = loop;
	node["Duration"] = std::to_string(duration);

	node["particlesScaleX"] = std::to_string(particlesScale.x);
	node["particlesScaleY"] = std::to_string(particlesScale.y);

	node["particleScaleRandomFactor"] = std::to_string(particlesScaleRandomFactor);


	node["Resources"]["ResourceTexture"];

	if (texture)
		node["Resources"]["ResourceTexture"] = std::string(texture->GetResourceFile());

	return node;
}

void ComponentParticleEmitter::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];

	//load the strings
	std::string Lsizex = node["sizeX"].is_null() ? "0" : node["sizeX"];
	std::string Lsizey = node["sizeY"].is_null() ? "0" : node["sizeY"];
	std::string Lsizez = node["sizeZ"].is_null() ? "0" : node["sizeZ"];

	std::string LemisionRate = node["emisionRate"].is_null() ? "0" : node["emisionRate"]; // typo: emission

	std::string LparticlesPerCreation = node["particlesPerCreation"].is_null() ? "1" : node["particlesPerCreation"];

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

	std::string LDuration = node["Duration"].is_null() ? "0" : node["Duration"];

	std::string LColorR = node["ColorR"].is_null() ? "0" : node["ColorR"];
	std::string LColorG = node["ColorG"].is_null() ? "0" : node["ColorG"];
	std::string LColorB = node["ColorB"].is_null() ? "0" : node["ColorB"];

	std::string LParticlesScaleX = node["particlesScaleX"].is_null() ? "1" : node["particlesScaleX"];
	std::string LParticlesScaleY = node["particlesScaleY"].is_null() ? "1" : node["particlesScaleY"];

	std::string LParticleScaleRandomFactor = node["particleScaleRandomFactor"].is_null() ? "1" : node["particleScaleRandomFactor"];

	if (!node["Loop"].is_null())
		loop = node["Loop"];
	else
		loop = true;

	std::string path = node["Resources"]["ResourceTexture"].is_null() ? "0" : node["Resources"]["ResourceTexture"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	ResourceTexture* auxText = (ResourceTexture*)App->resources->GetResource(std::stoi(path));
	
	if (auxText != nullptr)
		texture = auxText;

	if (texture)
		texture->AddUser(GO);

	//Pass the strings to the needed dada types
	size.x = std::stof(Lsizex);
	size.y = std::stof(Lsizey);
	size.z = std::stof(Lsizez);

	emisionRate = std::stof(LemisionRate);

	particlesPerCreation = std::stoi(LparticlesPerCreation);

	externalAcceleration.x = std::stof(LexternalAccelerationx);
	externalAcceleration.y = std::stof(LexternalAccelerationy);
	externalAcceleration.z = std::stof(LexternalAccelerationz);
	particleSystem->setExternalAcceleration(externalAcceleration);

	particlesVelocity.x = std::stof(LparticlesVelocityx);
	particlesVelocity.y = std::stof(LparticlesVelocityy);
	particlesVelocity.z = std::stof(LparticlesVelocityz);

	velocityRandomFactor.x = std::stof(LvelocityRandomFactorx);
	velocityRandomFactor.y = std::stof(LvelocityRandomFactory);
	velocityRandomFactor.z = std::stof(LvelocityRandomFactorz);

	particlesLifeTime = std::stof(LparticlesLifeTime);

	particlesSize = std::stof(LParticlesSize);

	particlesColor = float3(std::stof(LColorR), std::stof(LColorG), std::stof(LColorB));

	duration = std::stoi(LDuration);

	particlesScale.x = std::stof(LParticlesScaleX);
	particlesScale.y = std::stof(LParticlesScaleY);

	particlesScaleRandomFactor = std::stof(LParticleScaleRandomFactor);
}

void ComponentParticleEmitter::CreateInspectorNode()
{
		ImGui::Text("Loop");
		ImGui::SameLine();

		if (ImGui::Checkbox("##PELoop", &loop)) {
			if (loop)
				emisionActive = true;
		}

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

		ImGui::DragFloat("##SEmitterY", &size.y, 0.05f, 0.0f, 100.0f);

		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##SEmitterZ", &size.z, 0.05f, 0.0f, 100.0f);

		//External forces
		ImGui::Text("External forces ");
		bool forceChanged = false;
		//X
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		if (ImGui::DragFloat("##SExternalforcesX", &externalAcceleration.x, 0.05f, -50.0f, 50.0f))
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


		//Emision rate
		ImGui::Text("Emision rate (ms)");
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
		ImGui::DragFloat("##SEmision rate", &emisionRate, 1.0f, 1.0f, 100000.0f);

		//Emision rate
		ImGui::Text("Particles to create");
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
		ImGui::DragInt("##SParticlespercreation", &particlesPerCreation, 1.0f, 1.0f, 500.0f);

		//Particles lifetime
		ImGui::Text("Particles lifetime (ms)");
		ImGui::DragInt("##SParticlesLifetime", &particlesLifeTime, 3.0f, 0.0f, 10000.0f);
		
		ImGui::Separator();

		if (ImGui::TreeNode("Direction & velocity"))
		{
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

			ImGui::TreePop();
		}

		ImGui::Separator();
		
		if (ImGui::TreeNode("Renderer"))
		{
			//Scale
			ImGui::Text("Scale");

			//X
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##SParticlesScaleX", &particlesScale.x, 0.05f, 0.1f, 50.0f);

			ImGui::SameLine();
			//Y
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##SParticlesScaleY", &particlesScale.y, 0.05f, 0.1f, 50.0f);

			//Scale random factor
			ImGui::Text("Scale random factor");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##SParticlesRandomScaleX", &particlesScaleRandomFactor, 0.05f, 1.0f, 50.0f);

			// Image
			ImGui::Text("Image");

			if (texture == nullptr)
				ImGui::Image((ImTextureID)App->textures->GetDefaultTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //default texture
			else
				ImGui::Image((ImTextureID)texture->GetTexID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //loaded texture

			//drag and drop
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
				{
					uint UID = *(const uint*)payload->Data;
					Resource* resource = App->resources->GetResource(UID, false);

					if (resource && resource->GetType() == Resource::ResourceType::TEXTURE)
					{
						if (texture)
							texture->Release();

						texture = (ResourceTexture*)App->resources->GetResource(UID);
					}
				}
				ImGui::EndDragDropTarget();
			}
			////Particles Color

			ImGui::ColorEdit4("##PEParticle Color", (float*)&particlesColor, ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			ImGui::Text("Color");

			ImGui::TreePop();
		}
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
		spawnClock = App->time->GetGameplayTimePassed()*1000;

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

			//TESTING PERFORMANCE

			Quat velocityQuat = Quat(velocityBuffer[i].x, velocityBuffer[i].y, velocityBuffer[i].z,0);

			velocityQuat = rotation * velocityQuat * rotation.Conjugated();

			velocityBuffer[i] = physx::PxVec3(velocityQuat.x, velocityQuat.y, velocityQuat.z);

			//TESTING PERFORMANCE

			positionBuffer[i] = { physx::PxVec3(globalPosition.x + GetRandomValue(-size.x,size.x),
											globalPosition.y + GetRandomValue(-size.y,size.y),
											globalPosition.z + GetRandomValue(-size.z,size.z)) };

			particles[index[i]]->lifeTime = particlesLifeTime;
			particles[index[i]]->spawnTime = spawnClock;
			particles[index[i]]->color = particlesColor;
			particles[index[i]]->texture = texture;
			float randomScaleValue = GetRandomValue(1, particlesScaleRandomFactor);
			particles[index[i]]->scale.x = particlesScale.x *randomScaleValue;
			particles[index[i]]->scale.y = particlesScale.y * randomScaleValue;
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
	emisionStart = App->time->GetGameplayTimePassed() * 1000;
	spawnClock = emisionStart;
}

void ComponentParticleEmitter::Stop()
{
	emisionActive = false;
}

void ComponentParticleEmitter::SetLooping(bool active)
{
	loop = active;
}

void ComponentParticleEmitter::SetEmisionRate(float ms)
{
	emisionRate = ms;
}

void ComponentParticleEmitter::SetParticlesPerCreation(int particlesAmount)
{
	particlesPerCreation = particlesAmount;
}

void ComponentParticleEmitter::SetExternalAcceleration(float x, float y, float z)
{
	particleSystem->setExternalAcceleration(physx::PxVec3(x,y,z));
}

void ComponentParticleEmitter::SetParticlesVelocity(float x, float y, float z)
{
	particlesVelocity = physx::PxVec3(x,y,z);
}

void ComponentParticleEmitter::SetVelocityRF(float x, float y, float z)
{
	velocityRandomFactor = physx::PxVec3(x, y, z);
} 

void ComponentParticleEmitter::SetDuration(int duration)
{
	duration = duration;
}

void ComponentParticleEmitter::SetLifeTime(int ms)
{
	particlesLifeTime = ms;
}

void ComponentParticleEmitter::SetParticlesScale(float x, float y)
{
	particlesScale.x = x;
	particlesScale.y = y;
}

void ComponentParticleEmitter::SetParticlesScaleRF(float randomFactor)
{
	particlesScaleRandomFactor = randomFactor;
}
