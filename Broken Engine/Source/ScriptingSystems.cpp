#include "ScriptingSystems.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"

#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentAnimation.h"

#include "ComponentProgressBar.h"
#include "ComponentText.h"

#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"

#include "../Game/Assets/Sounds/Wwise_IDs.h"
#include "ComponentAudioSource.h"
#include "ModuleAudio.h"

#include "ScriptData.h"



using namespace Broken;
ScriptingSystems::ScriptingSystems() {}

ScriptingSystems::~ScriptingSystems() {}

// PHYSICS ------------------------------------------------------------
void ScriptingSystems::SetLinearVelocity(float x, float y, float z)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetLinearVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingSystems::SetAngularVelocity(float x, float y, float z)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetAngularVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingSystems::SetMass(float mass)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetMass(mass);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

float ScriptingSystems::GetMass()
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->GetMass();
	else
	{
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
		return 0.0f;
	}
}

int ScriptingSystems::GetLinearVelocity(lua_State* L)
{
	int ret = 0;
	float3 vel = float3(0.0f);
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
	{
		vel = body->GetLinearVelocity();
		ret = 3;
	}
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");

	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);
	lua_pushnumber(L, vel.z);
	return ret;
}

int ScriptingSystems::GetAngularVelocity(lua_State* L)
{
	int ret = 0;
	float3 vel = float3(0.0f);
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
	{
		vel = body->GetAngularVelocity();
		ret = 3;
	}
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");

	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);
	lua_pushnumber(L, vel.z);
	return ret;
}

void ScriptingSystems::AddForce(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddForce({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingSystems::AddTorque(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddTorque({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingSystems::SetKinematic(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->SetKinematic(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingSystems::UseGravity(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->UseGravity(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

// PARTICLES ----------------------------------------------------------
void ScriptingSystems::ActivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && !emmiter->IsEnabled())
	{
		emmiter->Enable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
	}
	else if (emmiter && emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingSystems::DeactivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && emmiter->IsEnabled())
	{
		emmiter->Disable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
	}
	else if (emmiter && !emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

// AUDIO --------------------------------------------------------------
void ScriptingSystems::PlayAttackSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_ATTACK);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_ATTACK);
		sound->wwiseGO->PlayEvent(AK::EVENTS::GERALT_ATTACK);
		sound->isPlaying = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingSystems::PlayStepSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_RUN);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_RUN);
		sound->wwiseGO->PlayEvent(AK::EVENTS::GERALT_RUN);
		sound->isPlaying = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingSystems::SetVolume(float volume)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	if (sound)
		sound->SetVolume(volume);
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingSystems::StopAttackSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_ATTACK);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_ATTACK);
		sound->isPlaying = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingSystems::StopStepSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_RUN);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_RUN);
		sound->isPlaying = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

// ANIMATIONS ----------------------------------------------------------
void ScriptingSystems::StartAnimation(const char* name, float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->PlayAnimation(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingSystems::SetCurrentAnimSpeed(float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetCurrentAnimationSpeed(speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}


void ScriptingSystems::SetAnimSpeed(const char* name, float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetAnimationSpeed(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

// UI --------------------------------------------------------------------
void ScriptingSystems::SetBarPercentage(float percentage)
{
	ComponentProgressBar* bar = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentProgressBar>();

	if (bar)
		bar->SetPercentage(percentage);
	else
		ENGINE_CONSOLE_LOG("[Script]: ProgressBar component is NULL");
}

void ScriptingSystems::SetUIText(const char* text)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText)
		CompText->SetText(text);
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component is NULL");
}

void ScriptingSystems::SetUITextAndNumber(const char* text, float number)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText && text)
	{
		//String streams aren't very performative, but we need them to keep OK the number's decimals
		std::ostringstream ss;
		ss << number;
		CompText->SetText((text + ss.str()).c_str());
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component or text passed is NULL");
}

void ScriptingSystems::SetUITextNumber(float number)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText)
	{
		//String streams aren't very performative, but we need them to keep OK the number's decimals
		std::ostringstream ss;
		ss << number;
		CompText->SetText(ss.str().c_str());
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component or text passed is NULL");
}