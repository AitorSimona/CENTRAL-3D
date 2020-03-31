#include "ScriptingTransform.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "Components.h"
#include "ScriptData.h"
#include "ResourceScene.h"

using namespace Broken;
ScriptingTransform::ScriptingTransform() {}

ScriptingTransform::~ScriptingTransform() {}

float ScriptingTransform::GetPositionX() const
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetPosition().x;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}

float ScriptingTransform::GetPositionY() const
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetPosition().y;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}

float ScriptingTransform::GetPositionZ() const
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetPosition().z;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}

int ScriptingTransform::GetPosition(lua_State* L)
{
	int ret = 0;
	float3 rot = float3(0.0f);

	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
	{
		rot = transform->GetPosition();
		ret = 3;
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");

	lua_pushnumber(L, rot.x);
	lua_pushnumber(L, rot.y);
	lua_pushnumber(L, rot.z);
	return ret;
}

void ScriptingTransform::Translate(float x, float y, float z, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
	{
		float3 trans_pos = transform->GetPosition();

		trans_pos.x += x;
		trans_pos.y += y;
		trans_pos.z += z;

		transform->SetPosition(trans_pos.x, trans_pos.y, trans_pos.z);
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingTransform::SetPosition(float x, float y, float z, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		transform->SetPosition(x, y, z);
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingTransform::RotateObject(float x, float y, float z)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	ComponentCollider* collider = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();
	ComponentDynamicRigidBody* rb = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();

	if (transform && rb && collider)
	{
		if (!rb->rigidBody)
			return;

		physx::PxTransform globalPos = rb->rigidBody->getGlobalPose();
		Quat quaternion = Quat::FromEulerXYZ(DEGTORAD * x, DEGTORAD * y, DEGTORAD * z);
		physx::PxQuat quat = physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		globalPos = physx::PxTransform(globalPos.p, quat);

		collider->UpdateTransformByRigidBody(rb, transform, &globalPos);

	}
	else if (transform)
	{
		float3 rot = transform->GetRotation();
		rot = float3(x, y, z);
		transform->SetRotation(rot);
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingTransform::SetObjectRotation(float x, float y, float z)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		transform->SetRotation({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingTransform::LookAt(float spotX, float spotY, float spotZ, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	ComponentCollider* collider = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();
	ComponentDynamicRigidBody* rb = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();

	if (transform)
	{
		float3 zaxis = float3(transform->GetGlobalPosition() - float3(spotX, spotY, spotZ)).Normalized();
		float3 xaxis = float3(zaxis.Cross(float3(0, 1, 0))).Normalized();
		float3 yaxis = xaxis.Cross(zaxis);
		zaxis = zaxis.Neg();

		float4x4 m = {
		   float4(xaxis.x, xaxis.y, xaxis.z, -Dot(xaxis, transform->GetGlobalPosition())),
		   float4(yaxis.x, yaxis.y, yaxis.z, -Dot(yaxis, transform->GetGlobalPosition())),
		   float4(zaxis.x, zaxis.y, zaxis.z, -Dot(zaxis, transform->GetGlobalPosition())),
		   float4(0, 0, 0, 1)
		};
		m.Transpose();

		float3 pos, scale;
		Quat rot;

		m.Decompose(pos, rot, scale);
		rot = rot.Inverted();

		if (rb && collider)
		{
			if (!rb->rigidBody)
				return;

			physx::PxTransform globalPos = rb->rigidBody->getGlobalPose();
			physx::PxQuat quat = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
			globalPos = physx::PxTransform(globalPos.p, quat);

			collider->UpdateTransformByRigidBody(rb, transform, &globalPos);
		}
		else
			transform->SetRotation(rot);
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

int ScriptingTransform::GetRotation(bool local, lua_State* L) const
{
	int ret = 0;
	float3 rot = float3(0.0f);
	
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
	{
		rot = transform->GetRotation();
		ret = 3;
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");

	lua_pushnumber(L, rot.x);
	lua_pushnumber(L, rot.y);
	lua_pushnumber(L, rot.z);
	return ret;
}

float ScriptingTransform::GetRotationX() const
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetRotation().x;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}

float ScriptingTransform::GetRotationY() const
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetRotation().y;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}

float ScriptingTransform::GetRotationZ() const
{	
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		return transform->GetRotation().z;
	else
	{
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
		return 0.0f;
	}
}