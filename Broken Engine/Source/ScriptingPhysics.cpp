#include "ScriptingPhysics.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ScriptData.h"

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