#include "ScriptingPhysics.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentCharacterController.h"
#include "ScriptData.h"
#include "ResourceScene.h"

using namespace Broken;
ScriptingPhysics::ScriptingPhysics() {}

ScriptingPhysics::~ScriptingPhysics() {}

// PHYSICS ------------------------------------------------------------
void ScriptingPhysics::SetLinearVelocity(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetLinearVelocity({ x, y, z });
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::SetAngularVelocity(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetAngularVelocity({ x, y, z });
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::SetMass(float mass, uint gameobject_UUID)
{	
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			body->SetMass(mass);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

float ScriptingPhysics::GetMass(uint gameobject_UUID)
{
	float ret = 0.0;
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			ret = body->GetMass();
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);

	return ret;
}

luabridge::LuaRef ScriptingPhysics::GetLinearVelocity(uint gameobject_UUID, lua_State* L)
{
	float3 vel = float3(0.0f); 
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
		{
			vel = body->GetLinearVelocity();
		}
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(vel.x);
	table.append(vel.y);
	table.append(vel.z);

	return table;
}

luabridge::LuaRef ScriptingPhysics::GetAngularVelocity(uint gameobject_UUID, lua_State* L)
{
	float3 vel = float3(0.0f);
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
		{
			vel = body->GetAngularVelocity();
		}
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(vel.x);
	table.append(vel.y);
	table.append(vel.z);

	return table;
}

void ScriptingPhysics::AddForce(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->AddForce({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::AddTorque(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->AddTorque({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::SetKinematic(bool enable, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->SetKinematic(enable);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::UseGravity(bool enable, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentDynamicRigidBody* body = go->GetComponent<ComponentDynamicRigidBody>();
		ComponentCollider* coll = go->GetComponent<ComponentCollider>();

		if (body && coll)
			return body->UseGravity(enable);
		else
			ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

luabridge::LuaRef ScriptingPhysics::GetCharacterPosition(uint gameobject_UUID, lua_State* L) {
	
	float3 aux = float3(0.0f);
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentCharacterController* character = go->GetComponent<ComponentCharacterController>();

		if (character) {
			physx::PxExtendedVec3 pos = character->controller->getFootPosition();
			aux.x = pos.x;
			aux.y = pos.y;
			aux.z = pos.z;
		}
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on GetCharacterPosition");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(aux.x);
	table.append(aux.y);
	table.append(aux.z);

	return table;
}

void ScriptingPhysics::SetCharacterPosition(float posx, float posy, float posz, uint gameobject_UUID) 
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentCharacterController* character = go->GetComponent<ComponentCharacterController>();
		if (character) {
			physx::PxExtendedVec3 pos(posx, posy, posz);

			character->controller->setFootPosition(pos);
		}
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on SetCharacterPosition");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

void ScriptingPhysics::Move(float vel_x, float vel_z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go != nullptr)
	{
		ComponentCharacterController* character = go->GetComponent<ComponentCharacterController>();
		if (character)
			character->SetVelocity(vel_x, 0, vel_z);
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on Move");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING): MoveGameObject function (physics) - Game Object not Found");
}

luabridge::LuaRef ScriptingPhysics::GetCharacterUpDirection(uint gameobject_UUID, lua_State* L)
{
	float3 aux = float3(0.0f);

	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentCharacterController* character = go->GetComponent<ComponentCharacterController>();

		if (character) {
			physx::PxVec3 dir = character->controller->getUpDirection();
			aux.x = dir.x;
			aux.y = dir.y;
			aux.z = dir.z;
		}
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on GetUpDirection");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(aux.x);
	table.append(aux.y);
	table.append(aux.z);

	return table;
}

void ScriptingPhysics::SetCharacterUpDirection(float rotx, float roty, float rotz, uint gameobject_UUID, lua_State* L)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentCharacterController* character = go->GetComponent<ComponentCharacterController>();
		if (character) {
			physx::PxVec3 rot(rotx, roty, rotz);

			character->controller->setUpDirection(rot);
		}
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on SetUpDirection");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject with %d UUID was not found!", gameobject_UUID);
}

luabridge::LuaRef ScriptingPhysics::OverlapSphere(float position_x, float position_y, float position_z, float radius, uint layer, lua_State* L)
{
	std::vector<uint> objects;
	App->physics->OverlapSphere(float3(position_x, position_y, position_z), radius, (LayerMask)layer, objects);

	luabridge::LuaRef ret = 0;
	luabridge::LuaRef uniforms(L, luabridge::newTable(L));
	for (int i = 0; i < objects.size(); ++i)
	{
		uniforms.append(objects[i]);
	}

	ret = uniforms;

	return ret;
}

int ScriptingPhysics::OnTriggerEnter(uint gameobject_UUID)
{
	int ret = 0; 
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	if (GO) {
		GameObject* other = GO->collisions.at(ONTRIGGER_ENTER);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}

int ScriptingPhysics::OnTriggerStay(uint gameobject_UUID)
{
	int ret = 0;
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (body) {
		GameObject* other = body->collisions.at(ONTRIGGER_STAY);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}

int ScriptingPhysics::OnTriggerExit(uint gameobject_UUID)
{
	int ret = 0;
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (body) {
		GameObject* other = body->collisions.at(ONTRIGGER_EXIT);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}

int ScriptingPhysics::OnCollisionEnter(uint gameobject_UUID)
{
	int ret = 0;
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_ENTER);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}
int ScriptingPhysics::OnCollisionStay(uint gameobject_UUID)
{
	int ret = 0;
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_STAY);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}

int ScriptingPhysics::OnCollisionExit(uint gameobject_UUID)
{
	int ret = 0;
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_EXIT);
		if (other) {
			ret = other->GetUID();
		}
	}

	return ret;
}