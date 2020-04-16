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
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->PlayAnimation(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetCurrentAnimSpeed(float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetCurrentAnimationSpeed(speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetAnimSpeed(const char* name, float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetAnimationSpeed(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetBlendTime(float value)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->ChangeBlendTime(value);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}