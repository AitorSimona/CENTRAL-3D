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

uint ScriptingGameobject::GetMyUID()
{
	return App->scripting->current_script->my_component->GetContainerGameObject()->GetUID();
}

uint ScriptingGameobject::GetScriptGOParent()
{
	uint ret = 0;
	GameObject* go = App->scripting->current_script->my_component->GetContainerGameObject()->parent;

	if (go && go->GetName() != App->scene_manager->GetName())
		ret = go->GetUID();
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! This Gameobject has no parent! 0 will be returned");

	return ret;
}

uint ScriptingGameobject::GetGOParentFromUID(uint gameobject_UUID)
{
	uint ret = 0;
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go && go->GetName() != App->scene_manager->GetName())
	{
		if (go->parent)
			ret = go->parent->GetUID();
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID has no parent! 0 will be returned", gameobject_UUID);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID was not found! 0 will be returned", gameobject_UUID);

	return ret;
}

void ScriptingGameobject::DestroyGOFromScript(uint gameobject_UUID)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go)
		App->scene_manager->SendToDelete(go);
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

void ScriptingGameobject::SetActiveGameObject(uint gameobject_UUID, bool active)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

	if (go) {
		if (active)
			go->Enable();
		else
			go->Disable();
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with %d UUID does not exist!", gameobject_UUID);
}

luabridge::LuaRef ScriptingGameobject::GetGameObjectPos(uint gameobject_UUID, lua_State* L)
{
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
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(rot.x);
	table.append(rot.y);
	table.append(rot.z);

	return table;
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
		go = (*App->scene_manager->currentScene->StaticGameObjects.find(gameobject_UUID)).second;

	if (go)
	{
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
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Object or its transformation component are null");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Game Object not found!");
}

uint ScriptingGameobject::GetComponentFromGO(uint gameobject_UUID, const char* component_name)
{
	uint ret = 0;
	GameObject* go = nullptr;
	go = App->scene_manager->currentScene->GetGOWithUID(gameobject_UUID);

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
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Component %s was not found inside Gameobject with UUID %d! 0 will be returned", component_name, gameobject_UUID);
		}
	}
	else
	{
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject with UUID %d was not found! 0 will be returned", gameobject_UUID);
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
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Current Active camera is NULL");

	return camlevel;
}

int ScriptingGameobject::GetFrustumPlanesIntersection(float x, float y, float z, float fovratio, lua_State* L)
{
	int ret = 0;

	// Top, Bottom, Left, Right
	double top = 1.0, bot = 1.0, left = 1.0, right = 1.0;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.TopPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			top = 0.0;
		if (sub1.BottomPlane().IsOnPositiveSide(pos))
			bot = 0.0;
		if (sub1.LeftPlane().IsOnPositiveSide(pos))
			left = 0.0;
		if (sub1.RightPlane().IsOnPositiveSide(pos))
			right = 0.0;

		ret = 4;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Current Active camera is NULL");

	lua_pushnumber(L, top);
	lua_pushnumber(L, bot);
	lua_pushnumber(L, left);
	lua_pushnumber(L, right);
	return ret;
}

int ScriptingGameobject::GetTopFrustumIntersection(float x, float y, float z, float fovratio)
{
	// Top
	int top = 1;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.TopPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			top = 0;
	}

	return top;
}

int ScriptingGameobject::GetBottomFrustumIntersection(float x, float y, float z, float fovratio)
{
	// Bot
	int bot = 1;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.BottomPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			bot = 0;
	}

	return bot;
}

int ScriptingGameobject::GetRightFrustumIntersection(float x, float y, float z, float fovratio)
{
	// Right
	int right = 1;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.RightPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			right = 0;
	}

	return right;
}

int ScriptingGameobject::GetLeftFrustumIntersection(float x, float y, float z, float fovratio)
{
	// Left
	int left = 1;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.LeftPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			left = 0;
	}

	return left;
}

luabridge::LuaRef ScriptingGameobject::WorldToScreen(float x, float y, float z, lua_State* L) {
	luabridge::LuaRef ret(L, luabridge::newTable(L));
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam) {
		float2 sc_coords = cam->WorldToScreen(float3(x, y, z));

		ret.append(sc_coords.x);
		ret.append(sc_coords.y);
	}

	return ret;
}

luabridge::LuaRef ScriptingGameobject::ScreenToWorld(float x, float y, float distance, lua_State* L) {
	luabridge::LuaRef ret(L, luabridge::newTable(L));
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam) {
		float3 wrld_coords = cam->ScreenToWorld(float2(x, y), distance);

		ret.append(wrld_coords.x);
		ret.append(wrld_coords.y);
		ret.append(wrld_coords.z);
	}

	return ret;
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


int ScriptingGameobject::GetMyLayer()
{
	GameObject* body = App->scripting->current_script->my_component->GetContainerGameObject();
	if (body) {
		return body->GetLayer();
	}
	return -1;
}

int ScriptingGameobject::GetLayerByID(uint UID)
{
	GameObject* body = App->scene_manager->currentScene->GetGOWithUID(UID);
	if (body) {
		return body->GetLayer();
	}
	return -1;
}
