#ifndef __SCRIPTINGINPUTS_H__
#define __SCRIPTINGINPUTS_H__

#include "ModuleInput.h"
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingInputs {
public:
	ScriptingInputs();
	~ScriptingInputs();

public:

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

};
BE_END_NAMESPACE
#endif // __SCRIPTINGINPUTS_H__
