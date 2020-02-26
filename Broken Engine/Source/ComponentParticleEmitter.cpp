#include "ComponentParticleEmitter.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleTimeManager.h"
#include "Imgui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"

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
{
	size = { 0,0,0 };
	emisionRate = 100.0f;

	particlesVelocity = { 0,0,0 };
	particlesDuration = 0;	
}

ComponentParticleEmitter::~ComponentParticleEmitter()
{}

void ComponentParticleEmitter::Enable()
{
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

			PxVec3 positionBuffer[] ={ PxVec3( globalPosition.x, globalPosition.y, globalPosition.y) };
			PxVec3 velocityBuffer[] = { particlesVelocity };

			creationData.indexBuffer = indexBuffer;
			creationData.positionBuffer = PxStrideIterator<const PxVec3>(positionBuffer);
			creationData.velocityBuffer = PxStrideIterator<const PxVec3>(velocityBuffer);

			bool succes = particleSystem->createParticles(creationData);
		}
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

	ImGui::DragFloat("##PX", &size.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##PY", &size.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##PZ", &size.z, 0.005f);

	//Emision rate
	ImGui::Text("Emision rate");
	ImGui::SameLine();
	ImGui::DragFloat("",&emisionRate, 0.05f);

	//Emision rate
	ImGui::Text("Max particles");
	ImGui::SameLine();
	ImGui::DragInt("", &maxParticles, 1);
}
