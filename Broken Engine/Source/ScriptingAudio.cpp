#include "ScriptingAudio.h"

// -- Modules --
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"
#include "ModuleRenderer3D.h"
#include "ModuleAudio.h"

// -- Components --
#include "GameObject.h"
#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentAnimation.h"
#include "ComponentCamera.h"
#include "ComponentAudioSource.h"


#include "../Game/Assets/Sounds/Wwise_IDs.h"
#include "ScriptData.h"

using namespace Broken;

ScriptingAudio::ScriptingAudio() {}

ScriptingAudio::~ScriptingAudio() {}

void ScriptingAudio::SetVolume(float volume)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	if (sound)
		sound->SetVolume(volume);
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingAudio::PlayAudioEvent(std::string event)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	uint EventId = App->audio->EventMap[event];


	sound->SetID(EventId);

	if (sound)
	{
		sound->wwiseGO->PlayEvent(EventId);
		sound->isPlaying = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingAudio::StopAudioEvent(std::string event)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	uint EventId = App->audio->EventMap[event];

	sound->SetID(EventId);

	if (sound)
	{
		sound->wwiseGO->StopEvent(EventId);
		sound->isPlaying = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingAudio::PauseAudioEvent(std::string event)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	uint EventId = App->audio->EventMap[event];

	sound->SetID(EventId);

	if (sound)
	{
		sound->wwiseGO->PauseEvent(EventId);
		sound->isPlaying = false;
		sound->isPaused = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void ScriptingAudio::ResumeAudioEvent(std::string event)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	uint EventId = App->audio->EventMap[event];

	sound->SetID(EventId);

	if (sound)
	{
		sound->wwiseGO->ResumeEvent(EventId);
		sound->isPlaying = true;
		sound->isPaused = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}