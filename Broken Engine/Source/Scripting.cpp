#include "Scripting.h"
#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "ModuleScripting.h"
#include "ComponentTransform.h"
#include "GameObject.h"


#include "ScriptData.h"



// ENGINE TRANSLATOR
// General
using namespace BrokenEngine;
Scripting::Scripting() {
}

Scripting::~Scripting() {
}

//Function that Lua will be able to call as LOG
void Scripting::LogFromLua(const char* string) {
	ENGINE_CONSOLE_LOG("[Script]: %s", string);
}

float Scripting::GetRealDT() const {
	return App->time->GetRealTimeDt();
}

float Scripting::GetDT() const {
	return App->time->GetGameDt();
}

// Input
int Scripting::GetKey(const char* key) const {
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return code;
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown key passed as string.");
		return -1;
	}
}

int Scripting::GetKeyState(const char* key) const {
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code);
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown key passed as string.");
		return -1;
	}
}

bool Scripting::IsKeyDown(const char* key) const {
	return GetKeyState(key) == KEY_DOWN;
}

bool Scripting::IsKeyUp(const char* key) const {
	return GetKeyState(key) == KEY_UP;
}

bool Scripting::IsKeyRepeat(const char* key) const {
	return GetKeyState(key) == KEY_REPEAT;
}

bool Scripting::IsKeyIdle(const char* key) const {
	return GetKeyState(key) == KEY_IDLE;
}

int Scripting::GetMouseButton(const char* button) const {
	if (!std::strcmp("Left", button))
		return SDL_BUTTON_LEFT;
	else if (!std::strcmp("Middle", button))
		return SDL_BUTTON_MIDDLE;
	else if (!std::strcmp("Right", button))
		return SDL_BUTTON_RIGHT;
	else if (!std::strcmp("X1", button))
		return SDL_BUTTON_X1;
	else if (!std::strcmp("X2", button))
		return SDL_BUTTON_X2;
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown MouseButton passed as string.");
		return -1;
	}
}

int Scripting::GetMouseButtonState(const char* button) const {
	if (!strcmp("Left", button))
		return App->input->GetMouseButton(SDL_BUTTON_LEFT);
	else if (!std::strcmp("Middle", button))
		return App->input->GetMouseButton(SDL_BUTTON_MIDDLE);
	else if (!std::strcmp("Right", button))
		return App->input->GetMouseButton(SDL_BUTTON_RIGHT);
	else if (!std::strcmp("X1", button))
		return App->input->GetMouseButton(SDL_BUTTON_X1);
	else if (!std::strcmp("X2", button))
		return App->input->GetMouseButton(SDL_BUTTON_X2);
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown MouseButton passed as string.");
		return -1;
	}
}

bool Scripting::IsMouseButtonDown(const char* button) const {
	return GetMouseButtonState(button) == KEY_DOWN;
}

bool Scripting::IsMouseButtonUp(const char* button) const {
	return GetMouseButtonState(button) == KEY_UP;
}

bool Scripting::IsMouseButtonRepeat(const char* button) const {
	return GetMouseButtonState(button) == KEY_REPEAT;
}

bool Scripting::IsMouseButtonIdle(const char* button) const {
	return GetMouseButtonState(button) == KEY_IDLE;
}

//bool Scripting::IsMouseInGame() const
//{
//	return !App->editor->using_menu;
//}
//
//int Scripting::GetMouseRaycastHit(lua_State *L)
//{
//	float3 hit = float3::zero;
//	const GameObject* go = App->camera->MousePick(&hit);
//
//	if (go != nullptr) {
//		lua_pushnumber(L, hit.x);
//		lua_pushnumber(L, hit.y);
//		lua_pushnumber(L, hit.z);
//	}
//
//	return 3;
//}
//
//// GameObjects
//GameObject* Scripting::FindGameObject(const char* objName) const
//{
//	return GOFunctions::FindModifiableGameObject(objName);
//}
//
//uint32_t Scripting::FindGameObjectUID(const char* objName) const
//{
//	return GOFunctions::FindGameObject(objName)->UID;
//}
//
//GameObject* Scripting::Instantiate(GameObject* reference, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale)
//{
//	GameObject* new_go = GOFunctions::InstantiateGameObject(reference);
//
//	if (new_go != nullptr) {
//		ComponentTransform* trs = (ComponentTransform*)new_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//		float3 rotRad = DegToRad(float3(rot_x, rot_y, rot_z));
//		trs->SetGlobalMat(float4x4::FromTRS(float3(pos_x, pos_y, pos_z), Quat::FromEulerXYZ(rotRad.x, rotRad.y, rotRad.z), float3(scale)));
//		trs->UpdateLocalMat();
//	}
//	else
//		LOG("[Script Error]: GameObject referenced was not found in scene.");
//
//	return new_go;
//}
//
//uint32_t Scripting::InstantiateByUID(uint32_t objUID, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale)
//{
//	GameObject* reference = GOFunctions::FindModifiableGameObject(objUID);
//
//	if (reference != nullptr)
//		return Instantiate(reference, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, scale)->UID;
//	else
//		return -1;
//}
//
//GameObject* Scripting::InstantiateByName(const char* objName, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale)
//{
//	GameObject* reference = GOFunctions::FindModifiableGameObject(objName);
//	return Instantiate(reference, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, scale);
//}
//
//GameObject* Scripting::Destroy(GameObject* target)
//{
//	return nullptr;
//}
//
//void Scripting::DestroyByUID(uint32_t objUID)
//{
//	App->scene_intro->MarkGameObjectDestruction(GOFunctions::FindModifiableGameObject(objUID));
//}
//
//void Scripting::Activate(bool state)
//{
//	App->scripting->current_script->my_component->active = state;
//}
//
//bool Scripting::IsActivated() const
//{
//	return App->scripting->current_script->my_component->active;
//}
//
//// OBJECT TRANSLATOR
//
//// General
//const GameObject* Scripting::GetGameObject() const
//{
//	return App->scripting->current_script->my_component->my_go;
//}
//
//const char* Scripting::GetObjectName() const
//{
//	return App->scripting->current_script->my_component->my_go->name.c_str();
//}
//
//void Scripting::ActivateObject(bool state)
//{
//	App->scripting->current_script->my_component->my_go->UpdateChildrenActive(state);
//}
//
//bool Scripting::IsObjectActivated() const
//{
//	return App->scripting->current_script->my_component->my_go->active;
//}
//
//void Scripting::SetStatic(bool state)
//{
//	App->scripting->current_script->my_component->my_go->UpdateStaticStatus(state);
//}
//
//bool Scripting::IsStatic() const
//{
//	return App->scripting->current_script->my_component->my_go->staticObj;
//}
//
//void Scripting::DestroySelf() const
//{
//	App->scene_intro->MarkGameObjectDestruction(App->scripting->current_script->my_component->my_go);
//}
//
// Position
float Scripting::GetPositionX() const {
	return App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>()->GetPosition().x;//GetComponent(COMPONENT_TYPE::TRANSFORM))->position.x;
}

float Scripting::GetPositionY() const {
	return App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>()->GetPosition().y;
}

float Scripting::GetPositionZ() const {
	return App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>()->GetPosition().z;
}

int Scripting::GetPosition(bool local, lua_State* L) const {
	ComponentTransform* trs = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	float3 pos;

	pos = trs->GetPosition();

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	return 3;
}

void Scripting::Translate(float x, float y, float z, bool local) {
	ComponentTransform* trs = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	float3 trans_pos = trs->GetPosition();
	trans_pos.x += x;
	trans_pos.y += y;
	trans_pos.z += z;
	trs->SetPosition(trans_pos.x, trans_pos.y, trans_pos.z);
}

void Scripting::SetPosition(float x, float y, float z, bool local) {
	ComponentTransform* trs = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	trs->SetPosition(x, y, z);
}

//// Rotation
//float Scripting::GetEulerX(bool local) const
//{
//	if (local) {
//		return RadToDeg(((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.x);
//	}
//	else {
//		float3x3 auxMat;
//		((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().Decompose(auxMat, float3());
//		return RadToDeg(auxMat.ToEulerXYZ().x);
//	}
//}
//
//float Scripting::GetEulerY(bool local) const
//{
//	if (local) {
//		return RadToDeg(((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.y);
//	}
//	else {
//		float3x3 auxMat;
//		((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().Decompose(auxMat, float3());
//		return RadToDeg(auxMat.ToEulerXYZ().y);
//	}
//		
//}
//
//float Scripting::GetEulerZ(bool local) const
//{
//	if (local) {
//		return RadToDeg(((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.z);
//	}
//	else {
//		float3x3 auxMat;
//		((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().Decompose(auxMat, float3());
//		return RadToDeg(auxMat.ToEulerXYZ().z);
//	}
//}
//
//int Scripting::GetEulerRotation(bool local, lua_State *L) const
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	float3x3 auxMat;
//	float3 rot;
//
//	if (local)
//		trs->localTrs.RotatePart().Decompose(auxMat, float3());
//	else
//		trs->globalTrs.RotatePart().Decompose(auxMat, float3());
//
//	rot = RadToDeg(auxMat.ToEulerXYZ());
//
//	lua_pushnumber(L, rot.x);
//	lua_pushnumber(L, rot.y);
//	lua_pushnumber(L, rot.z);
//
//	return 3;
//}
//
//void Scripting::RotateByEuler(float x, float y, float z, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	trs->Rotate(DegToRad(float3(x, y, z)), local);
//}
//
//void Scripting::SetEulerRotation(float x, float y, float z, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	trs->SetRotation(DegToRad(float3(x, y, z)), local);
//}
//
//float Scripting::GetQuatX(bool local) const
//{
//	if (local)
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->localTrs.RotatePart().ToQuat().x;
//	else {
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToQuat().x;
//	}
//}
//
//float Scripting::GetQuatY(bool local) const
//{
//	if (local)
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->localTrs.RotatePart().ToQuat().y;
//	else
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToQuat().y;
//}
//
//float Scripting::GetQuatZ(bool local) const
//{
//	if (local)
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->localTrs.RotatePart().ToQuat().z;
//	else
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToQuat().z;
//}
//
//float Scripting::GetQuatW(bool local) const
//{
//	if (local)
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->localTrs.RotatePart().ToQuat().w;
//	else
//		return ((ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToQuat().w;
//}
//
//int Scripting::GetQuatRotation(bool local, lua_State *L) const
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	float4 rot;
//	Quat q;
//
//	if (local) {
//		q = trs->localTrs.RotatePart().ToQuat();
//		rot = float4(q.x, q.y, q.z, q.w);
//	}
//	else {
//		q = trs->globalTrs.RotatePart().ToQuat();
//		rot = float4(q.x, q.y, q.z, q.w);
//	}
//
//	lua_pushnumber(L, rot.x);
//	lua_pushnumber(L, rot.y);
//	lua_pushnumber(L, rot.z);
//	lua_pushnumber(L, rot.w);
//
//	return 4;
//}
//
//void Scripting::RotateByQuat(float x, float y, float z, float w, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	trs->Rotate(Quat(x, y, z, w), local);
//}
//
//void Scripting::SetQuatRotation(float x, float y, float z, float w, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	trs->SetRotation(Quat(x, y, z, w), local);
//}
//
//// Others
//void Scripting::LookAt(float spotX, float spotY, float spotZ, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	float3 dir;
//
//	if (local)
//		dir = (float3(spotX, spotY, spotZ) - trs->localTrs.TranslatePart());
//	else
//		dir = (float3(spotX, spotY, spotZ) - trs->globalTrs.TranslatePart());
//
//	LookTo(dir.x, dir.y, dir.z, local);
//}
//
//void Scripting::LookTo(float dirX, float dirY, float dirZ, bool local)
//{
//	ComponentTransform* trs = (ComponentTransform*)App->scripting->current_script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
//
//	if (local) {
//		trs->localTrs = trs->localTrs * Quat::RotateFromTo(trs->localTrs.WorldZ(), float3(dirX, dirY, dirZ).Normalized());
//	}
//	else {
//		ComponentTransform* parentTrs = (ComponentTransform*)trs->my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);
//		trs->localTrs = trs->localTrs * Quat::LookAt(trs->localTrs.WorldZ(), float3(dirX, dirY, dirZ).Normalized(), trs->localTrs.WorldY(), float3::unitY);
//		trs->localTrs = parentTrs->localTrs.Inverted().RotatePart().ToQuat() * trs->localTrs;	// Maybe there's a simpler way, but this works and by God I'm not fucking touching it again for now
//
//		trs->rotation = trs->localTrs.ToEulerXYZ();
//		//trs->globalTrs = trs->globalTrs * Quat::RotateFromTo(trs->globalTrs.WorldZ().Normalized(), float3(dirX, dirY, dirZ).Normalized());
//		//trs->needsUpdateLocal = true;
//	}
//
//	trs->needsUpdateGlobal = true;
//}

