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

void ScriptingAnimations::StartAnimation(const char* name, float speed, uint gameobject_UUID)
{	
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	ComponentAnimation* anim = go->GetComponent<ComponentAnimation>();

	if (anim)
		anim->PlayAnimation(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetCurrentAnimSpeed(float speed, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	ComponentAnimation* anim = go->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetCurrentAnimationSpeed(speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetAnimSpeed(const char* name, float speed, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	ComponentAnimation* anim = go->GetComponent<ComponentAnimation>();
	if (anim)
		anim->SetAnimationSpeed(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void ScriptingAnimations::SetBlendTime(float value, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	ComponentAnimation* anim = go->GetComponent<ComponentAnimation>();
	if (anim)
		anim->ChangeBlendTime(value);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}