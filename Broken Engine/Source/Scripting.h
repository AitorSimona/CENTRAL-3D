#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__
#include "ModuleInput.h"

class lua_State;

class Scripting
{
public:
	Scripting();
	~Scripting();

public:
	// ENGINE TRANSLATOR
	// General

	//--------------------------------------------------Comented all functions for now, since we are testing in the 1st place the module itself------------------------//
	void LogFromLua(const char* string);

	float GetRealDT() const;
	float GetDT() const;
	float GameTime();

	// Input
	int GetKey(const char* key) const;
	int GetKeyState(const char* key) const;

	bool IsKeyDown(const char* key) const;
	bool IsKeyUp(const char* key) const;
	bool IsKeyRepeat(const char* key) const;
	bool IsKeyIdle(const char* key) const;

	int GetMouseButton(const char* button) const;
	int GetMouseButtonState(const char* button) const;

	bool IsMouseButtonDown(const char* button) const;
	bool IsMouseButtonUp(const char* button) const;
	bool IsMouseButtonRepeat(const char* button) const;
	bool IsMouseButtonIdle(const char* button) const;

	bool IsGamepadButton(int player_num, const char* button, const char* button_state) const;
	SDL_GameControllerButton GetControllerButtonFromString(const char* button_name) const;
	GP_BUTTON_STATE GetGamepadButtonState(const char* state_name) const;

	bool IsJoystickAxis(int player_num, const char* joy_axis, const char* axis_state) const;
	bool IsTriggerState(int player_num, const char* trigger, const char* button_state) const;
	SDL_GameControllerAxis GetControllerAxisFromString(const char* axis_name) const;
	GP_AXIS_STATE GetAxisStateFromString(const char* state_name) const;

	int GetAxisRealValue(int player_num, const char* joy_axis) const;
	float GetAxisValue(int player_num, const char* joy_axis, float threshold) const;
	void ShakeController(int player_num, float intensity, uint32 milliseconds) const;
	void StopControllerShake(int player_num) const;

	uint FindGameObject(const char* go_name);
	float GetGameObjectPos(uint gameobject_UID, lua_State* L);
	float GetGameObjectPosX(uint gameobject_UID);
	float GetGameObjectPosY(uint gameobject_UID);
	float GetGameObjectPosZ(uint gameobject_UID);

	void TranslateGameObject(uint gameobject_UID, float x, float y, float z);

	//bool IsMouseInGame() const;
	//int GetMouseRaycastHit(lua_State *L);

	//// GameObjects
	//GameObject* FindGameObject(const char* objName) const;
	//uint32_t FindGameObjectUID(const char* objName) const;

	//GameObject* Instantiate(GameObject* reference, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
	//uint32_t InstantiateByUID(uint32_t objUID, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
	//GameObject* InstantiateByName(const char* objName, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);

	//GameObject* Destroy(GameObject* target);
	//void DestroyByUID(uint32_t objUID);

	//// SCRIPT TRANSLATOR
	//void Activate(bool state);
	//bool IsActivated() const;

	//// OBJECT TRANSLATOR
	//// General
	//const GameObject* GetGameObject() const;
	//const char* GetObjectName() const;

	//void ActivateObject(bool state);
	//bool IsObjectActivated() const;

	//void SetStatic(bool state);
	//bool IsStatic() const;

	//void DestroySelf() const;

	// SYSTEMS FUNCTIONS --------------------------------------------------------------------------------------------------------- //
	void ActivateParticlesEmission() const;
	void DeactivateParticlesEmission() const;

	// Position
	float GetPositionX() const;
	float GetPositionY() const;
	float GetPositionZ() const;
	int GetPosition(lua_State *L);

	void Translate(float x, float y, float z, bool local);
	void SetPosition(float x, float y, float z, bool local);

	//Rotation
	void RotateObject(float x, float y, float z);
	void SetObjectRotation(float x, float y, float z);
	void LookAt(float spotX, float spotY, float spotZ, bool local);

	int GetRotation(bool local, lua_State* L) const;
	float GetRotationX() const;
	float GetRotationY() const;
	float GetRotationZ() const;

	//Physics
	void SetMass(float mass);
	float GetMass();

	int GetLinearVelocity(lua_State* L);
	int GetAngularVelocity(lua_State* L);

	void SetLinearVelocity(float x, float y, float z);
	void SetAngularVelocity(float x, float y, float z);

	void AddTorque(float forceX, float forceY, float forceZ, int ForceMode);
	void AddForce(float forceX, float forceY, float forceZ, int ForceMode);

	void SetKinematic(bool enable);
	void UseGravity(bool enable);

	//Audio
	void PlayAttackSound();
	void PlayStepSound();
	void StopAttackSound();
	void StopStepSound();



	void SetVolume(float volume);

	//Animations
	void StartAnimation(const char* name, float speed);

	//// Rotation
	//float GetEulerX(bool local) const;	// Roll
	//float GetEulerY(bool local) const;	// Pitch
	//float GetEulerZ(bool local) const;	// Yaw
	//int GetEulerRotation(bool local, lua_State *L) const;
	//
	//void RotateByEuler(float x, float y, float z, bool local);
	//void SetEulerRotation(float x, float y, float z, bool local);

	////DON'T USE, quat methods need revision
	//float GetQuatX(bool local) const;
	//float GetQuatY(bool local) const;
	//float GetQuatZ(bool local) const;
	//float GetQuatW(bool local) const;
	//int GetQuatRotation(bool local, lua_State *L) const;

	//void RotateByQuat(float x, float y, float z, float w, bool local);
	//void SetQuatRotation(float x, float y, float z, float w, bool local);

	//// Others
	//void LookAt(float posX, float posY, float posZ, bool local);
	//void LookTo(float dirX, float dirY, float dirZ, bool local);
};
#endif // !__SCRIPTING_H__
