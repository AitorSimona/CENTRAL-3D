#include "ScriptingElements.h"
#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"
#include "ComponentTransform.h"
#include "GameObject.h"

#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentAudioSource.h"
#include "ComponentAnimation.h"

#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"

#include "../Game/Assets/Sounds/Wwise_IDs.h"
#include "ComponentAudioSource.h"
#include "ModuleAudio.h"

#include "ScriptData.h"

using namespace Broken;
ScriptingElements::ScriptingElements() {}

ScriptingElements::~ScriptingElements() {}

// Returns the UID of the GameObject if it is found
uint ScriptingElements::FindGameObject(const char* go_name)
{
	uint ret = 0;

	GameObject* go = App->scene_manager->currentScene->GetGOWithName(go_name);

	if (go != nullptr)
	{
		ret = go->GetUID();
	}
	else
	{
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject %s was not found! 0 will be returned", go_name);
	}
	return ret;
}

float ScriptingElements::GetGameObjectPos(uint gameobject_UID, lua_State* L)
{
	float ret = 0;
	float3 rot = float3(0.0f);

	GameObject* go = (*App->scene_manager->currentScene->NoStaticGameObjects.find(gameobject_UID)).second;
	if (go == nullptr)
	{
		go = (*App->scene_manager->currentScene->StaticGameObjects.find(gameobject_UID)).second;
	}

	ComponentTransform* transform;
	transform = go->GetComponent<ComponentTransform>();
	if (go != nullptr && transform != nullptr)
	{
		rot = transform->GetPosition();
		ret = 3;
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");

	lua_pushnumber(L, rot.x);
	lua_pushnumber(L, rot.y);
	lua_pushnumber(L, rot.z);
	return 0.0f;
}

float ScriptingElements::GetGameObjectPosX(uint gameobject_UID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UID);

	if (GO != nullptr)
	{
		ComponentTransform* transform;
		transform = GO->GetComponent<ComponentTransform>();

		if (transform)
		{
			ret = transform->GetGlobalPosition().x;
		}
	}

	return ret;
}

float ScriptingElements::GetGameObjectPosY(uint gameobject_UID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UID);

	if (GO != nullptr)
	{
		ComponentTransform* transform;
		transform = GO->GetComponent<ComponentTransform>();

		if (transform)
		{
			ret = transform->GetGlobalPosition().y;
		}
	}

	return ret;
}

float ScriptingElements::GetGameObjectPosZ(uint gameobject_UID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UID);

	if (GO != nullptr)
	{
		ComponentTransform* transform;
		transform = GO->GetComponent<ComponentTransform>();

		if (transform)
		{
			ret = transform->GetGlobalPosition().z;
		}
	}

	return ret;
}

void ScriptingElements::TranslateGameObject(uint gameobject_UID, float x, float y, float z)
{
	GameObject* go = (*App->scene_manager->currentScene->NoStaticGameObjects.find(gameobject_UID)).second;
	if (go == nullptr)
	{
		go = (*App->scene_manager->currentScene->StaticGameObjects.find(gameobject_UID)).second;
	}

	ComponentTransform* transform;
	transform = go->GetComponent<ComponentTransform>();

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

float ScriptingElements::GetPositionX() const
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

float ScriptingElements::GetPositionY() const
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

float ScriptingElements::GetPositionZ() const
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

int ScriptingElements::GetPosition(lua_State* L)
{
	int ret = 0;
	float3 rot = float3(0.0f);

	ComponentTransform* transform;
	if ((transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>()))
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

void ScriptingElements::Translate(float x, float y, float z, bool local)
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

void ScriptingElements::SetPosition(float x, float y, float z, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		transform->SetPosition(x, y, z);
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingElements::RotateObject(float x, float y, float z)
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

void ScriptingElements::SetObjectRotation(float x, float y, float z)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		transform->SetRotation({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void ScriptingElements::LookAt(float spotX, float spotY, float spotZ, bool local)
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

int ScriptingElements::GetRotation(bool local, lua_State* L) const
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

float ScriptingElements::GetRotationX() const
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

float ScriptingElements::GetRotationY() const
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

float ScriptingElements::GetRotationZ() const
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

// CAMERAS -----------------------------------------------------------
int ScriptingElements::GetPosInFrustum(float x, float y, float z)
{
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
		return (int)cam->frustum.Contains({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("[Script]: Current Active camera is NULL");

	return 0;
}
