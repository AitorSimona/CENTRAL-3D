#include "ScriptingPhysics.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ScriptData.h"
#include "ResourceScene.h"

using namespace Broken;
ScriptingPhysics::ScriptingPhysics() {}

ScriptingPhysics::~ScriptingPhysics() {}

// PHYSICS ------------------------------------------------------------
void ScriptingPhysics::SetLinearVelocity(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetLinearVelocity({ x, y, z });
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingPhysics::SetAngularVelocity(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();
	
	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetAngularVelocity({ x, y, z });
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingPhysics::SetMass(float mass, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetMass(mass);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

float ScriptingPhysics::GetMass(uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->GetMass();
		else
		{
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
			return 0.0f;
		}
	}
	else {
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
		return 0.0f;
	}
}

int ScriptingPhysics::GetLinearVelocity(uint gameobject_UUID, lua_State* L)
{
	int ret = 0;
	float3 vel = float3(0.0f); 

	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
		{
			vel = body->GetLinearVelocity();
			ret = 3;
		}
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);

	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);
	lua_pushnumber(L, vel.z);
	return ret;
}

int ScriptingPhysics::GetAngularVelocity(uint gameobject_UUID, lua_State* L)
{
	int ret = 0;
	float3 vel = float3(0.0f);

	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
		{
			vel = body->GetAngularVelocity();
			ret = 3;
		}
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);

	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);
	lua_pushnumber(L, vel.z);
	return ret;
}

void ScriptingPhysics::AddForce(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->AddForce({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingPhysics::AddTorque(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->AddTorque({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingPhysics::SetKinematic(bool enable, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->SetKinematic(enable);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingPhysics::UseGravity(bool enable, uint gameobject_UUID)
{
	GameObject* go = nullptr;

	if (gameobject_UUID != -1)
		go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	else
		go = App->scripting->current_script->my_component->GetContainerGameObject();

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->UseGravity(enable);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}