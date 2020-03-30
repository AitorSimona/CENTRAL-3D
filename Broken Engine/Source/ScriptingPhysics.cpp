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

void ScriptingPhysics::SetMass(float mass)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

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

int ScriptingPhysics::GetLinearVelocity(lua_State* L)
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

int ScriptingPhysics::GetAngularVelocity(lua_State* L)
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

void ScriptingPhysics::AddForce(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddForce({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
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

void ScriptingPhysics::Move(float vel_x, float vel_y) {
	ComponentCharacterController* character = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCharacterController>();
	if (character) {
		character->Move(vel_x, vel_y);
	}
}
void ScriptingPhysics::OverlapSphere(lua_State* L)
{
	int count = 1;
	int posx = lua_tointeger(L, ++count);
	int posy = lua_tointeger(L, ++count);
	int posz = lua_tointeger(L, ++count);
	int radius = lua_tointeger(L, ++count);
	uint layer = lua_tointeger(L, ++count);

	std::vector<uint> objects;
	App->physics->OverlapSphere(float3(posx, posy, posz), radius, (LayerMask)layer, objects);

	lua_getglobal(L, "getcollisions");
	lua_newtable(L);
	for (size_t i = 0; i < objects.size(); ++i) {
		lua_pushinteger(L, i + 1);
		lua_pushnumber(L, objects[i]);
		lua_settable(L, -3);
	}
}
int ScriptingPhysics::OverlapSphere2(float position_x, float position_y, float position_z, float radius, uint layer, lua_State* L)
{
	std::vector<uint> objects;
	App->physics->OverlapSphere(float3(position_x, position_y, position_z), radius, (LayerMask)layer, objects);

	//lua_newtable(L);

	/*for (int i = 1; i <= objects.size(); i++) {
		lua_pushnumber(L, 123456789.0f);
			return 1;
		lua_rawseti(L, -2, i); 
	}	*/	
	lua_pushnumber(L, 5);
	return 1;

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
