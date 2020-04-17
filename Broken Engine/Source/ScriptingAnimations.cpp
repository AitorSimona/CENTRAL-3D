#include "ScriptingAnimations.h"

// -- Modules --
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"

// -- Components --
#include "GameObject.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentAnimation.h"

#include "ResourceScene.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingAnimations::ScriptingAnimations() {}

ScriptingAnimations::~ScriptingAnimations() {}

void ScriptingAnimations::StartAnimation(const char* name, float speed)
{
	ComponentAnimation* anim = nullptr;

	if(App->scripting->current_script)
		anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();
	else
		ENGINE_CONSOLE_LOG("[Script]: Script component is NULL");

	if (anim)
		anim->PlayAnimation(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetCurrentAnimSpeed(float speed)
{
	ComponentAnimation* anim = nullptr;

	if (App->scripting->current_script)
		anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();
	else
		ENGINE_CONSOLE_LOG("[Script]: Script component is NULL");

	if (anim)
		anim->SetCurrentAnimationSpeed(speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetAnimSpeed(const char* name, float speed)
{
	ComponentAnimation* anim = nullptr;

	if (App->scripting->current_script)
		anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();
	else
		ENGINE_CONSOLE_LOG("[Script]: Script component is NULL");

	if (anim)
		anim->SetAnimationSpeed(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetBlendTime(float value)
{
	ComponentAnimation* anim = nullptr;

	if (App->scripting->current_script)
		anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();
	else
		ENGINE_CONSOLE_LOG("[Script]: Script component is NULL");

	if (anim)
		anim->ChangeBlendTime(value);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

int ScriptingAnimations::CurrentAnimEnded()
{
	ComponentAnimation* anim = nullptr;

	if (App->scripting->current_script)
		anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();
	else
		ENGINE_CONSOLE_LOG("[Script]: Script component is NULL");

	if (anim)
		return (int)anim->CurrentAnimationEnded();
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");

	return 0;
}