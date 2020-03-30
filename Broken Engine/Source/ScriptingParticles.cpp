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
void ScriptingParticles::ActivateParticleEmitter(uint gameobject_UUID) const
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emmitter = go->GetComponent<ComponentParticleEmitter>();

		if (emmitter && !emmitter->IsEnabled())
		{
			emmitter->Enable();
			ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
		}
		else if (emmitter && emmitter->IsEnabled())
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::DeactivateParticleEmitter(uint gameobject_UUID) const
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emmitter = go->GetComponent<ComponentParticleEmitter>();

		if (emmitter && emmitter->IsEnabled())
		{
			emmitter->Disable();
			ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
		}
		else if (emmitter && !emmitter->IsEnabled())
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
		else
			ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::PlayParticleEmitter(uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->Play();
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::StopParticleEmitter(uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->Stop();
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetEmissionRateFromScript(float ms, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetEmisionRate(ms);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticlesPerCreationFromScript(int particlesAmount, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetParticlesPerCreation(particlesAmount);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleAcceleration(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetExternalAcceleration(x, y, z);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleVelocityFromScript(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetParticlesVelocity(x, y, z);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetRandomParticleVelocity(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetVelocityRF(x, y, z);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleLooping(bool active, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetLooping(active);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleDuration(int duration, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetDuration(duration);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleLifeTime(int ms, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetLifeTime(ms);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetParticleScaleFromScript(float x, float y, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetParticlesScale(x, y);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingParticles::SetRandomParticleScale(float randomFactor, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentParticleEmitter* emitter = go->GetComponent<ComponentParticleEmitter>();
		emitter->SetParticlesScaleRF(randomFactor);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}