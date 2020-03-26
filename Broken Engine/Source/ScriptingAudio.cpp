#include "ScriptingAudio.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"

#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentAnimation.h"

#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"

#include "../Game/Assets/Sounds/Wwise_IDs.h"
#include "ComponentAudioSource.h"
#include "ModuleAudio.h"

#include "ScriptData.h"

using namespace Broken;
ScriptingAudio::ScriptingAudio() {}

ScriptingAudio::~ScriptingAudio() {}

void ScriptingAudio::PlayAttackSound()
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

void ScriptingAudio::PlayStepSound()
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

void ScriptingAudio::SetVolume(float volume)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	if (sound)
		sound->SetVolume(volume);
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingAudio::StopAttackSound()
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

void ScriptingAudio::StopStepSound()
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