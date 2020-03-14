#include "Scripting.h"
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



// ENGINE TRANSLATOR
// General
using namespace Broken;
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

float Scripting::GameTime()
{
	return App->time->GetGameplayTimePassed();
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

bool Scripting::IsGamepadButton(int player_num, const char* button, const char* button_state) const
{
	bool ret = false;
	//Get Player
	PLAYER player = PLAYER::P1;
	if(player_num > 0)
	player = (PLAYER)(player_num -1);

	//Get Button
	SDL_GameControllerButton SDL_button = GetControllerButtonFromString(button);

	//Get Button State
	GP_BUTTON_STATE b_state = GetGamepadButtonState(button_state);

	//Evaluate condition
	if (App->input->GetButton(player, (int)SDL_button) == b_state)
		ret = true;
	else
		ret = false;

	return ret;
}

//Get the SDL_Controller_Button for a given String
SDL_GameControllerButton Scripting::GetControllerButtonFromString(const char* button_name) const
{
	SDL_GameControllerButton button;

	if (!strcmp("BUTTON_INVALID", button_name))
		button = SDL_CONTROLLER_BUTTON_INVALID;
	else if (!std::strcmp("BUTTON_A", button_name))
		button = SDL_CONTROLLER_BUTTON_A;
	else if (!std::strcmp("BUTTON_B", button_name))
		button = SDL_CONTROLLER_BUTTON_B;
	else if (!std::strcmp("BUTTON_X", button_name))
		button = SDL_CONTROLLER_BUTTON_X;
	else if (!std::strcmp("BUTTON_Y", button_name))
		button = SDL_CONTROLLER_BUTTON_Y;
	else if (!std::strcmp("BUTTON_BACK", button_name))
		button = SDL_CONTROLLER_BUTTON_BACK;
	else if (!std::strcmp("BUTTON_GUIDE", button_name))
		button = SDL_CONTROLLER_BUTTON_GUIDE;
	else if (!std::strcmp("BUTTON_START", button_name))
		button = SDL_CONTROLLER_BUTTON_START;
	else if (!std::strcmp("BUTTON_LEFTSTICK", button_name))
		button = SDL_CONTROLLER_BUTTON_LEFTSTICK;
	else if (!std::strcmp("BUTTON_RIGHTSTICK", button_name))
		button = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	else if (!std::strcmp("BUTTON_LEFTSHOULDER", button_name))
		button = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	else if (!std::strcmp("BUTTON_RIGHTSHOULDER", button_name))
		button = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	else if (!std::strcmp("BUTTON_DPAD_UP", button_name))
		button = SDL_CONTROLLER_BUTTON_DPAD_UP;
	else if (!std::strcmp("BUTTON_DPAD_DOWN", button_name))
		button = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	else if (!std::strcmp("BUTTON_DPAD_LEFT", button_name))
		button = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	else if (!std::strcmp("BUTTON_DPAD_RIGHT", button_name))
		button = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	else
		button = SDL_CONTROLLER_BUTTON_INVALID;

	return button;
}

GP_BUTTON_STATE Scripting::GetGamepadButtonState(const char* state_name) const
{
	GP_BUTTON_STATE ret = GP_BUTTON_STATE::BUTTON_IDLE;

	if (!strcmp("IDLE", state_name))
		ret = GP_BUTTON_STATE::BUTTON_IDLE;
	else if (!std::strcmp("DOWN", state_name))
		ret = GP_BUTTON_STATE::BUTTON_DOWN;
	else if (!std::strcmp("REPEAT", state_name))
		ret = GP_BUTTON_STATE::BUTTON_REPEAT;
	else if (!std::strcmp("UP", state_name))
		ret = GP_BUTTON_STATE::BUTTON_UP;

	return ret;
}

bool Scripting::IsJoystickAxis(int player_num, const char* joy_axis, const char* axis_state) const
{
	bool ret = false;
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);

	//Get Axis
	SDL_GameControllerAxis SDL_axis = GetControllerAxisFromString(joy_axis);

	if (SDL_axis == SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
		SDL_axis == SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
	{
		//In case anyone tries to call this function to use the triggers
		ENGINE_CONSOLE_LOG("Tried to call IsJoystickAxis with a Trigger! this function is exclusive for joysticks, FALSE will be returned by default");
	}
	else
	{
		//Get State
		GP_AXIS_STATE SDL_axis_state = GetAxisStateFromString(axis_state);

		if (App->input->GetLRAxisState(player, SDL_axis) == SDL_axis_state)
			ret = true;
	}

	return ret;
}

bool Scripting::IsTriggerState(int player_num, const char* trigger, const char* button_state) const
{
	bool ret = false;
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);

	//Get TRIGGER
	SDL_GameControllerAxis SDL_axis = GetControllerAxisFromString(trigger);

	if (SDL_axis != SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT &&
		SDL_axis != SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
	{
		//In case anyone tries to call this function to use the triggers
		ENGINE_CONSOLE_LOG("Tried to call Gamepad INPUT function IsTriggerState with a joystick axis! this function is exclusive for Triggers, FALSE will be returned by default");
	}
	else
	{
		//Get State
		GP_BUTTON_STATE SDL_axis_state = GetGamepadButtonState(button_state);

		if (App->input->GetTriggerState(player, SDL_axis) == SDL_axis_state)
			ret = true;
	}

	return ret;
}

SDL_GameControllerAxis Scripting::GetControllerAxisFromString(const char* axis_name) const
{
	SDL_GameControllerAxis ret= SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_INVALID;

	if (!strcmp("AXIS_INVALID", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_INVALID;
	else if (!std::strcmp("AXIS_LEFTX", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX;
	else if (!std::strcmp("AXIS_LEFTY", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY;
	else if (!std::strcmp("AXIS_RIGHTX", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX;
	else if (!std::strcmp("AXIS_RIGHTY", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY;
	else if (!std::strcmp("AXIS_TRIGGERLEFT", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	else if (!std::strcmp("AXIS_TRIGGERRIGHT", axis_name))
		ret = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

	return ret;
}

GP_AXIS_STATE Scripting::GetAxisStateFromString(const char* state_name) const
{
	GP_AXIS_STATE ret = GP_AXIS_STATE::AXIS_IDLE;

	if (!strcmp("IDLE", state_name))
		ret = GP_AXIS_STATE::AXIS_IDLE;
	else if (!std::strcmp("POSITIVE_DOWN", state_name))
		ret = GP_AXIS_STATE::AXIS_POSITIVE_DOWN;
	else if (!std::strcmp("POSITIVE_REPEAT", state_name))
		ret = GP_AXIS_STATE::AXIS_POSITIVE_REPEAT;
	else if (!std::strcmp("POSITIVE_RELEASE", state_name))
		ret = GP_AXIS_STATE::AXIS_POSITIVE_RELEASE;
	else if (!std::strcmp("NEGATIVE_DOWN", state_name))
		ret = GP_AXIS_STATE::AXIS_NEGATIVE_DOWN;
	else if (!std::strcmp("NEGATIVE_REPEAT", state_name))
		ret = GP_AXIS_STATE::AXIS_NEGATIVE_REPEAT;
	else if (!std::strcmp("NEGATIVE_RELEASE", state_name))
		ret = GP_AXIS_STATE::AXIS_NEGATIVE_RELEASE;

	return ret;
}

int Scripting::GetAxisRealValue(int player_num, const char* axis) const
{
	int ret = 0;

	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);

	//Get Axis (you can use both triggers & axis for this function)
	SDL_GameControllerAxis SDL_axis = GetControllerAxisFromString(axis);

	ret = App->input->GetAxis(player,SDL_axis);
	ENGINE_CONSOLE_LOG("%i", ret);
	return ret;
}

//Get the value of an axis from -1.0f to 1.0f if the value in the gamepad surpasses the threshold, else returns 0.0f
float Scripting::GetAxisValue(int player_num, const char* joy_axis, float threshold) const
{
	float ret = 0.0f;

	//App->input->SetAxisThreshold(threshold);
	int val = GetAxisRealValue(player_num, joy_axis);

	ret = val / (float)AXISMAX;

	if (IN_RANGE(ret, -threshold, threshold))
		ret = 0.0f; //In case the value is lower than the threshold, (both from the negative & positive sides) we return 0.0f (no imput, implementing a deadzone basically)

	return ret;
}

void Scripting::ShakeController(int player_num, float intensity, uint32 milliseconds) const
{
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);
	App->input->ShakeController((PLAYER)player, intensity, milliseconds);
}

void Scripting::StopControllerShake(int player_num) const
{
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);
	App->input->StopControllerShake((PLAYER)player);
}

//Returns the UID of the GameObject if it is found
uint Scripting::FindGameObject(const char* go_name)
{
	uint ret = 0;

	GameObject* go = App->scene_manager->currentScene->GetGOWithName(go_name);

	if (go != nullptr)
	{
		ret = go->GetUID();
	}
	else
	{
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Gameobject %s was not found! 0 will be returned",go_name);
	}
	return ret;
}

float Scripting::GetGameObjectPos(uint gameobject_UID, lua_State* L)
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
	if ( go != nullptr  && transform != nullptr)
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

float Scripting::GetGameObjectPosX(uint gameobject_UID)
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

float Scripting::GetGameObjectPosY(uint gameobject_UID)
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

float Scripting::GetGameObjectPosZ(uint gameobject_UID)
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

void Scripting::TranslateGameObject(uint gameobject_UID, float x, float y, float z)
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
float Scripting::GetPositionX() const
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

float Scripting::GetPositionY() const
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

float Scripting::GetPositionZ() const
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

int Scripting::GetPosition(lua_State *L)
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

void Scripting::Translate(float x, float y, float z, bool local)
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

void Scripting::SetPosition(float x, float y, float z, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if (transform)
		transform->SetPosition(x, y, z);
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void Scripting::RotateObject(float x, float y, float z)
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

	}else if (transform)
	{
		float3 rot = transform->GetRotation();
		rot = float3(x, y, z);
		transform->SetRotation(rot);
	}
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}


void Scripting::SetObjectRotation(float x, float y, float z)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();

	if(transform)
		transform->SetRotation({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its transformation component are null");
}

void Scripting::LookAt(float spotX, float spotY, float spotZ, bool local)
{
	ComponentTransform* transform = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentTransform>();
	ComponentCollider* collider = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();
	ComponentDynamicRigidBody* rb = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();

	if (transform)
	{
		float3 zaxis = float3(transform->GetGlobalPosition() - float3(spotX, spotY, spotZ)).Normalized();
		float3 xaxis = float3(zaxis.Cross(float3(0,1,0))).Normalized();
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

int Scripting::GetRotation(bool local, lua_State* L) const
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

float Scripting::GetRotationX() const
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

float Scripting::GetRotationY() const
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

float Scripting::GetRotationZ() const
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


// ------------------------ SYSTEMS FUNCTIONS ------------------------ //
//PHYSICS ------------------------------------------------------------
void Scripting::SetLinearVelocity(float x, float y, float z)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetLinearVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void Scripting::SetAngularVelocity(float x, float y, float z)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetAngularVelocity({ x, y, z });
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void Scripting::SetMass(float mass)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		body->SetMass(mass);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

float Scripting::GetMass()
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

int Scripting::GetLinearVelocity(lua_State* L)
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

int Scripting::GetAngularVelocity(lua_State* L)
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

void Scripting::AddForce(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddForce({forceX, forceY, forceZ}, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void Scripting::AddTorque(float forceX, float forceY, float forceZ, int ForceMode)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->AddTorque({ forceX, forceY, forceZ }, (physx::PxForceMode::Enum)ForceMode);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void Scripting::SetKinematic(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->SetKinematic(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

void Scripting::UseGravity(bool enable)
{
	ComponentDynamicRigidBody* body = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentDynamicRigidBody>();
	ComponentCollider* coll = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentCollider>();

	if (body && coll)
		return body->UseGravity(enable);
	else
		ENGINE_CONSOLE_LOG("Object or its Dynamic Rigid Body component or its Collider are null");
}

//PARTICLES ----------------------------------------------------------
void Scripting::ActivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && !emmiter->IsEnabled())
	{
		emmiter->Enable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
	}
	else if (emmiter && emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void Scripting::DeactivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && emmiter->IsEnabled())
	{
		emmiter->Disable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
	}
	else if (emmiter && !emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}


//AUDIO --------------------------------------------------------------
void Scripting::PlayAttackSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_ATTACK);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_ATTACK);
		sound->wwiseGO->PlayEvent(AK::EVENTS::GERALT_ATTACK);
		sound->isPlaying = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void Scripting::PlayStepSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_RUN);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_RUN);
		sound->wwiseGO->PlayEvent(AK::EVENTS::GERALT_RUN);
		sound->isPlaying = true;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void Scripting::SetVolume(float volume)
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();

	if (sound)
		sound->SetVolume(volume);
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void Scripting::StopAttackSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_ATTACK);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_ATTACK);
		sound->isPlaying = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

void Scripting::StopStepSound()
{
	ComponentAudioSource* sound = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAudioSource>();
	sound->SetID(AK::EVENTS::GERALT_RUN);

	if (sound)
	{
		sound->wwiseGO->StopEvent(AK::EVENTS::GERALT_RUN);
		sound->isPlaying = false;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Sound Emmiter component is NULL");
}

//ANIMATION ----------------------------------------------------------
void Scripting::StartAnimation(const char* name, float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->PlayAnimation(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

void Scripting::SetCurrentAnimSpeed(float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetCurrentAnimationSpeed(speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}


void Scripting::SetAnimSpeed(const char* name, float speed)
{
	ComponentAnimation* anim = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentAnimation>();

	if (anim)
		anim->SetAnimationSpeed(name, speed);
	else
		ENGINE_CONSOLE_LOG("[Script]: Animation component is NULL");
}

// CAMERAS -----------------------------------------------------------
int Scripting::GetPosInFrustum(float x, float y, float z, float fovratio1, float fovratio2)
{
	ComponentCamera* cam = App->renderer3D->active_camera;

	int camlevel = 0;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1, sub2 = cam->frustum;
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

// MATHS -------------------------------------------------------------
//Maths
int Scripting::FloatNumsAreEqual(float a, float b)
{
	return (fabs(a - b) < std::numeric_limits<float>::epsilon());
}

int Scripting::DoubleNumsAreEqual(double a, double b)
{
	return (fabs(a - b) < std::numeric_limits<double>::epsilon());
}

// ------------------------------------------------------------------- //


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

