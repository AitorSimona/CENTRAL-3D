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
		sound->wwiseGO->StopEvent(EventId);
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