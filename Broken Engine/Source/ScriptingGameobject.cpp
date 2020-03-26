#include "ScriptingGameobject.h"
#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ComponentScript.h"
#include "ModuleRenderer3D.h"
#include "ScriptData.h"
#include "ModuleEventManager.h"

using namespace Broken;
ScriptingGameobject::ScriptingGameobject() {}

ScriptingGameobject::~ScriptingGameobject() {}

// Returns the UID of the GameObject if it is found
uint ScriptingGameobject::FindGameObject(const char* go_name)
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

void ScriptingGameobject::DestroyGOFromScript(uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);
	ENGINE_CONSOLE_LOG("Destroying: %s ...", go->GetName());	

	App->scene_manager->SendToDelete(go); 
}

float ScriptingGameobject::GetGameObjectPos(uint gameobject_UUID, lua_State* L)
{
	float ret = 0;
	float3 rot = float3(0.0f);

	GameObject* go = (*App->scene_manager->currentScene->NoStaticGameObjects.find(gameobject_UUID)).second;
	if (go == nullptr)
	{
		go = (*App->scene_manager->currentScene->StaticGameObjects.find(gameobject_UUID)).second;
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

float ScriptingGameobject::GetGameObjectPosX(uint gameobject_UUID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

float ScriptingGameobject::GetGameObjectPosY(uint gameobject_UUID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

float ScriptingGameobject::GetGameObjectPosZ(uint gameobject_UUID)
{
	float ret = 0.0f;
	GameObject* GO = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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

void ScriptingGameobject::TranslateGameObject(uint gameobject_UUID, float x, float y, float z)
{
	GameObject* go = (*App->scene_manager->currentScene->NoStaticGameObjects.find(gameobject_UUID)).second;
	if (go == nullptr)
	{
		go = (*App->scene_manager->currentScene->StaticGameObjects.find(gameobject_UUID)).second;
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

uint ScriptingGameobject::GetComponentFromGO(const char* component_name, const char* go_name)
{
	uint ret = 0;
	GameObject* go = nullptr;

	if (go_name == "NO_NAME")
		go = App->scripting->current_script->my_component->GetContainerGameObject();
	else
		go = App->scene_manager->currentScene->GetGOWithName(go_name);

	if (go != nullptr)
	{
		Component* comp = nullptr;
		std::string name = component_name;

		if (name.compare("Animation") == 0)
			comp = (Component*)go->GetComponent<ComponentAnimation>();
		else if (name.compare("Audio") == 0)
			comp = (Component*)go->GetComponent<ComponentAudioSource>();
		else if (name.compare("Bone") == 0)
			comp = (Component*)go->GetComponent<ComponentBone>();
		else if (name.compare("Camera") == 0)
			comp = (Component*)go->GetComponent<ComponentCamera>();
		else if (name.compare("Collider") == 0)
			comp = (Component*)go->GetComponent<ComponentCollider>();
		else if (name.compare("Rigidbody") == 0)
			comp = (Component*)go->GetComponent<ComponentDynamicRigidBody>();
		else if (name.compare("Mesh") == 0)
			comp = (Component*)go->GetComponent<ComponentMesh>();
		else if (name.compare("Emitter") == 0)
			comp = (Component*)go->GetComponent<ComponentParticleEmitter>();
		else if (name.compare("Text") == 0)
			comp = (Component*)go->GetComponent<ComponentText>();
		else if (name.compare("Bar") == 0)
			comp = (Component*)go->GetComponent<ComponentProgressBar>();
		else if (name.compare("Button") == 0)
			comp = (Component*)go->GetComponent<ComponentButton>();
		else if (name.compare("Image") == 0)
			comp = (Component*)go->GetComponent<ComponentImage>();
		else if (name.compare("Canvas") == 0)
			comp = (Component*)go->GetComponent<ComponentCanvas>();

		if (comp != nullptr)
		{
			ret = comp->GetUID();
		}
		else
		{
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Component %s was not found inside Gameobject %s! 0 will be returned", component_name, go_name);
		}
	}
	else
	{
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject %s was not found! 0 will be returned", go_name);
	}

	return ret;
}

// CAMERAS -----------------------------------------------------------
int ScriptingGameobject::GetPosInFrustum(float x, float y, float z, float fovratio1, float fovratio2)
{
	ComponentCamera* cam = App->renderer3D->active_camera;
	int camlevel = 0;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		Frustum sub2 = cam->frustum;

		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio1, cam->frustum.AspectRatio());
		sub2.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio2, cam->frustum.AspectRatio());

		// --- Check in which subdivision we are ---
		if ((int)cam->frustum.Contains({ x, y, z }) == true)
		{
			camlevel = 1;

			if ((int)sub1.Contains({ x, y, z }) == true)
			{
				camlevel = 2;

				if ((int)sub2.Contains({ x, y, z }) == true)
				{
					camlevel = 3;
				}
			}
		}
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Current Active camera is NULL");

	return camlevel;
}

int ScriptingGameobject::GetFrustumPlanesIntersection(float x, float y, float z, lua_State* luaSt)
{
	ComponentCamera* cam = App->renderer3D->active_camera;
	if (cam)
	{
		float3 pos = { x, y, z };
		int T, B, L, R;		//Top, Bottom, Left, Right
		T = B = L = R = 1;	//Considered to be inside the frustum (at planes' negative side) by default

		if (cam->frustum.TopPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			T = 0;
		if (cam->frustum.BottomPlane().IsOnPositiveSide(pos))
			B = 0;
		if (cam->frustum.LeftPlane().IsOnPositiveSide(pos))
			L = 0;
		if (cam->frustum.RightPlane().IsOnPositiveSide(pos))
			R = 0;

		lua_pushnumber(luaSt, T);
		lua_pushnumber(luaSt, B);
		lua_pushnumber(luaSt, L);
		lua_pushnumber(luaSt, R);
		return 4;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Current Active camera is NULL");

	return 0;
}


luabridge::LuaRef ScriptingGameobject::GetScript(uint gameobject_UUID, lua_State* L)
{
	luabridge::LuaRef ret = 0;

	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go != nullptr)
	{
		ComponentScript* component_script = go->GetComponent<ComponentScript>();
		ScriptInstance* script = App->scripting->GetScriptInstanceFromComponent(component_script);

		ret = script->my_table_class;
	}
	return ret;
}
