#include "ScriptingTransform.h"

// -- Modules --
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"

// -- Components -
#include "GameObject.h"
#include "Components.h"
#include "ComponentTransform.h"

#include "ScriptData.h"
#include "ResourceScene.h"

using namespace Broken;
ScriptingTransform::ScriptingTransform() {}

ScriptingTransform::~ScriptingTransform() {}

luabridge::LuaRef ScriptingTransform::GetPosition(uint gameobject_UUID, lua_State* L)
{
	float3 pos = float3(0.0f);
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();

		if (transform)
		{
			pos = transform->GetPosition();
		}
		else
			ENGINE_CONSOLE_LOG("Object or its transformation component are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);

	/*luabridge::LuaRef table = luabridge::newTable(L);
	table.append(pos.x);
	table.append(pos.y);
	table.append(pos.z);*/
	//luaL_dofile(L,)

	luabridge::LuaRef ScriptGetTable = luabridge::getGlobal(L, "NewVector3");
	luabridge::LuaRef table(ScriptGetTable());

	double x = pos.x;
	double y = pos.y;
	double z = pos.z;

	double test = 20.0f;

	std::string x_str = "x";
	std::string y_str = "y";
	std::string z_str = "z";
	

	std::map<std::string, double> nums_test;
	nums_test.emplace("test",30.0f);

	nums_test["test"] = 50.0f;

	if(table[x_str.c_str()].isNumber())
	{
	table[x_str.c_str()] = pos.x;
	table[y_str.c_str()] = pos.y;
	table[z_str.c_str()] = pos.z;
	}

	//for (luabridge::Iterator iterator(table); !iterator.isNil(); ++iterator) {
	//	// Declare necessary vars for intialization & get variable name
	//	std::string str = (*iterator).first.tostring();

	//	if (str.data() == "x")
	//	{
	//		(*iterator).second = pos.x;
	//	}
	//	else if (str.data() == "y")
	//	{
	//		(*iterator).second = pos.y;
	//	}
	//	else if (str.data() == "z")
	//	{
	//		(*iterator).second = pos.z;
	//	}
	//}
	

	return table;
}

void ScriptingTransform::Translate(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();

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
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);
}

void ScriptingTransform::SetPosition(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();

		if (transform)
			transform->SetPosition(x, y, z);
		else
			ENGINE_CONSOLE_LOG("Object or its transformation component are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);
}

void ScriptingTransform::RotateObject(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();
		ComponentCollider* collider = go->GetComponent<ComponentCollider>();
		ComponentDynamicRigidBody* rb = go->GetComponent<ComponentDynamicRigidBody>();

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
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);
}

void ScriptingTransform::SetObjectRotation(float x, float y, float z, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();

		if (transform)
			transform->SetRotation({ x, y, z });
		else
			ENGINE_CONSOLE_LOG("Object or its transformation component are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);
}

void ScriptingTransform::LookAt(float spotX, float spotY, float spotZ, uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();
		ComponentCollider* collider = go->GetComponent<ComponentCollider>();
		ComponentDynamicRigidBody* rb = go->GetComponent<ComponentDynamicRigidBody>();

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
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);
}

luabridge::LuaRef ScriptingTransform::GetRotation(uint gameobject_UUID, lua_State* L)
{
	float3 rot = float3(0.0f);	
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		ComponentTransform* transform = go->GetComponent<ComponentTransform>();

		if (transform)
			rot = transform->GetRotation();
		else
			ENGINE_CONSOLE_LOG("Object or its transformation component are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", gameobject_UUID);

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(rot.x);
	table.append(rot.y);
	table.append(rot.z);

	return table;
}