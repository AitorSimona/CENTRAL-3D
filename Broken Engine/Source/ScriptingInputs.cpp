#include "ScriptingInputs.h"
#include "Application.h"

using namespace Broken;
ScriptingInputs::ScriptingInputs() {}

ScriptingInputs::~ScriptingInputs() {}

int ScriptingInputs::GetKey(const char* key) const {
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return code;
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown key passed as string.");
		return -1;
	}
}

int ScriptingInputs::GetKeyState(const char* key) const {
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code);
	else {
		ENGINE_CONSOLE_LOG("[Script Error]: Unknown key passed as string.");
		return -1;
	}
}

bool ScriptingInputs::IsKeyDown(const char* key) const {
	return GetKeyState(key) == KEY_DOWN;
}

bool ScriptingInputs::IsKeyUp(const char* key) const {
	return GetKeyState(key) == KEY_UP;
}

bool ScriptingInputs::IsKeyRepeat(const char* key) const {
	return GetKeyState(key) == KEY_REPEAT;
}

bool ScriptingInputs::IsKeyIdle(const char* key) const {
	return GetKeyState(key) == KEY_IDLE;
}

int ScriptingInputs::GetMouseButton(const char* button) const {
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

int ScriptingInputs::GetMouseButtonState(const char* button) const {
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

bool ScriptingInputs::IsMouseButtonDown(const char* button) const {
	return GetMouseButtonState(button) == KEY_DOWN;
}

bool ScriptingInputs::IsMouseButtonUp(const char* button) const {
	return GetMouseButtonState(button) == KEY_UP;
}

bool ScriptingInputs::IsMouseButtonRepeat(const char* button) const {
	return GetMouseButtonState(button) == KEY_REPEAT;
}

bool ScriptingInputs::IsMouseButtonIdle(const char* button) const {
	return GetMouseButtonState(button) == KEY_IDLE;
}

bool ScriptingInputs::IsGamepadButton(int player_num, const char* button, const char* button_state) const
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
SDL_GameControllerButton ScriptingInputs::GetControllerButtonFromString(const char* button_name) const
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

GP_BUTTON_STATE ScriptingInputs::GetGamepadButtonState(const char* state_name) const
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

bool ScriptingInputs::IsJoystickAxis(int player_num, const char* joy_axis, const char* axis_state) const
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

bool ScriptingInputs::IsTriggerState(int player_num, const char* trigger, const char* button_state) const
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

SDL_GameControllerAxis ScriptingInputs::GetControllerAxisFromString(const char* axis_name) const
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

GP_AXIS_STATE ScriptingInputs::GetAxisStateFromString(const char* state_name) const
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

int ScriptingInputs::GetAxisRealValue(int player_num, const char* axis) const
{
	int ret = 0;

	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);

	//Get Axis (you can use both triggers & axis for this function)
	SDL_GameControllerAxis SDL_axis = GetControllerAxisFromString(axis);

	ret = App->input->GetAxis(player,SDL_axis);
	//ENGINE_CONSOLE_LOG("%i", ret);
	return ret;
}

//Get the value of an axis from -1.0f to 1.0f if the value in the gamepad surpasses the threshold, else returns 0.0f
float ScriptingInputs::GetAxisValue(int player_num, const char* joy_axis, float threshold) const
{
	float ret = 0.0f;

	//App->input->SetAxisThreshold(threshold);
	int val = GetAxisRealValue(player_num, joy_axis);

	ret = val / (float)AXISMAX;

	if (IN_RANGE(ret, -threshold, threshold))
		ret = 0.0f; //In case the value is lower than the threshold, (both from the negative & positive sides) we return 0.0f (no imput, implementing a deadzone basically)

	return ret;
}

void ScriptingInputs::ShakeController(int player_num, float intensity, uint32 milliseconds) const
{
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);
	App->input->ShakeController((PLAYER)player, intensity, milliseconds);
}

void ScriptingInputs::StopControllerShake(int player_num) const
{
	//Get Player
	PLAYER player = PLAYER::P1;
	if (player_num > 0)
		player = (PLAYER)(player_num - 1);
	App->input->StopControllerShake((PLAYER)player);
}