#include "ScriptingParticles.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ResourceScene.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingParticles::ScriptingParticles() {}

ScriptingParticles::~ScriptingParticles() {}

// PARTICLES ----------------------------------------------------------
void ScriptingParticles::ActivateParticleEmitter() const
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emitter && !emitter->IsEnabled())
	{
		emitter->Enable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
	}
	else if (emitter && emitter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::DeactivateParticleEmitter() const
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emitter && emitter->IsEnabled())
	{
		emitter->Disable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
	}
	else if (emitter && !emitter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::ActivateParticleEmitterGO(uint gameobject_UUID) const
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();

		if (emitter && !emitter->IsEnabled())
		{
			emitter->Enable();
			ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
		}
		else if (emitter && emitter->IsEnabled())
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingParticles::DeactivateParticleEmitterGO(uint gameobject_UUID) const
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();

		if (emitter && emitter->IsEnabled())
		{
			emitter->Disable();
			ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
		}
		else if (emitter && !emitter->IsEnabled())
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}


void ScriptingParticles::PlayParticleEmitter()
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->Play();
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::StopParticleEmitter()
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->Stop();
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::PlayParticleEmitterGO(uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		if (emitter)
			emitter->Play();
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: GameObject with UUID %d could not be found!", gameobject_UUID);
}

void ScriptingParticles::StopParticleEmitterGO(uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		if (emitter)
			emitter->Stop();
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: GameObject with UUID %d could not be found!", gameobject_UUID);
}

void ScriptingParticles::SetEmissionRateFromScript(float ms)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetEmisionRate(ms);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticlesPerCreationFromScript(int particlesAmount)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetParticlesPerCreation(particlesAmount);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleAcceleration(float x, float y, float z)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetExternalAcceleration(x, y, z);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleVelocityFromScript(float x, float y, float z)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetParticlesVelocity(x, y, z);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetRandomParticleVelocity(float x, float y, float z)
{	
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetVelocityRF(x, y, z);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleLooping(bool active)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetLooping(active);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleDuration(int duration)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetDuration(duration);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleLifeTime(int ms)
{	
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetLifeTime(ms);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetParticleScaleFromScript(float x, float y)
{
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetParticlesScale(x, y);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::SetRandomParticleScale(float randomFactor)
{	
	ComponentParticleEmitter* emitter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();
	if (emitter)
		emitter->SetParticlesScaleRF(randomFactor);
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}