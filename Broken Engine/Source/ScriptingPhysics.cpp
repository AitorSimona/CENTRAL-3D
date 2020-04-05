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
void ScriptingPhysics::SetLinearVelocity(float x, float y, float z)
{	
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetLinearVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingPhysics::SetAngularVelocity(float x, float y, float z)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetAngularVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingPhysics::SetLinearVelocityGO(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

void ScriptingPhysics::SetAngularVelocityGO(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

void ScriptingPhysics::SetMass(float mass)
{	
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()-> GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetMass(mass);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

float ScriptingPhysics::GetMass()
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

luabridge::LuaRef ScriptingPhysics::GetLinearVelocity(lua_State* L)
{
	float3 vel = float3(0.0f); 

	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
	{
		vel = body->GetLinearVelocity();
	}
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(vel.x);
	table.append(vel.y);
	table.append(vel.z);

	return table;
}

luabridge::LuaRef ScriptingPhysics::GetAngularVelocity(lua_State* L)
{
	float3 vel = float3(0.0f);

	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
	{
		vel = body->GetAngularVelocity();
	}
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(vel.x);
	table.append(vel.y);
	table.append(vel.z);

	return table;
}

void ScriptingPhysics::AddForce(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddForce({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingPhysics::AddForceGO(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

void ScriptingPhysics::AddTorque(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddTorque({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingPhysics::AddTorqueGO(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID)
{
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

void ScriptingPhysics::SetKinematic(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->SetKinematic(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void ScriptingPhysics::UseGravity(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->UseGravity(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

luabridge::LuaRef ScriptingPhysics::GetCharacterPosition(lua_State* L) {
	
	float3 aux = float3(0.0f);
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();

	if (character) {
		physx::PxExtendedVec3 pos = character->controller->getFootPosition();
		aux.x = pos.x;
		aux.y = pos.y;
		aux.z = pos.z;
	}
	else
		ENGINE_CONSOLE_LOG("Character Controller is null on GetCharacterPosition");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(aux.x);
	table.append(aux.y);
	table.append(aux.z);

	return table;

}

void ScriptingPhysics::SetCharacterPosition(float posx, float posy, float posz) {
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();
	if (character) {
		physx::PxExtendedVec3 pos(posx, posy, posz);

		character->controller->setFootPosition(pos);
	}
	else
		ENGINE_CONSOLE_LOG("Character Controller is null on SetCharacterPosition");
}

void ScriptingPhysics::Move(float vel_x, float vel_y)
{
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();
	if (character)
		character->Move(vel_x, vel_y);
	else
		ENGINE_CONSOLE_LOG("Character Controller is null on Move");
}

void ScriptingPhysics::MoveGameObject(uint gameobject_UUID, float vel_x, float vel_y)
{
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (GO != nullptr)
	{
		ComponentCharacterController* character = GO->GetComponent<ComponentCharacterController>();
		if (character)
			character->Move(vel_x, vel_y);
		else
			ENGINE_CONSOLE_LOG("Character Controller is null on Move");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING): MoveGameObject function (physics) - Game Object not Found");
}

luabridge::LuaRef ScriptingPhysics::GetCharacterUpDirection(lua_State* L)
{
	float3 aux = float3(0.0f);
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();

	if (character) {
		physx::PxVec3 dir = character->controller->getUpDirection();
		aux.x = dir.x;
		aux.y = dir.y;
		aux.z = dir.z;
	}
	else
		ENGINE_CONSOLE_LOG("Character Controller is null on GetUpDirection");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(aux.x);
	table.append(aux.y);
	table.append(aux.z);

	return table;
}

void ScriptingPhysics::SetCharacterUpDirection(float rotx, float roty, float rotz, lua_State* L)
{
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();
	if (character) {
		physx::PxVec3 rot(rotx, roty, rotz);

		character->controller->setUpDirection(rot);
	}
	else
		ENGINE_CONSOLE_LOG("Character Controller is null on SetUpDirection");
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

int ScriptingPhysics::OnTriggerEnter(uint UID, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONTRIGGER_ENTER);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}

int ScriptingPhysics::OnTriggerStay(uint UID, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONTRIGGER_STAY);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}

int ScriptingPhysics::OnTriggerExit(uint UID, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONTRIGGER_EXIT);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}

int ScriptingPhysics::OnCollisionEnter(uint id, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_ENTER);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}
int ScriptingPhysics::OnCollisionStay(uint UID, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_STAY);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}

int ScriptingPhysics::OnCollisionExit(uint UID, lua_State* L)
{
	int ret = 0;
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		GameObject* other = body->collisions.at(ONCOLLISION_EXIT);
		if (other) {
			ret = other->GetUID();
		}
	}
	return ret;
}
