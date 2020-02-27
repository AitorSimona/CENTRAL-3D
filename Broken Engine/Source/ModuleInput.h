#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5
#define NUM_GAMEPAD_BUTTONS 15
// This macro controls how many gamepads at max will be processed 
#define MAX_GAMEPADS 4
#define AXISMAX 32767

struct _SDL_GameController;
struct _SDL_Haptic;
struct SDL_GameControllerButtonBind;

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

// Gamepad input state (just for better legibility)
enum GP_BUTTON_STATE
{
	BUTTON_IDLE = 0,
	BUTTON_DOWN,
	BUTTON_REPEAT,
	BUTTON_UP
};


enum class BUTTON_BIND
{
	BASIC_ATTACK = 0,
	SPECIAL_ATTACK,
	SUPER_ATTACK,
	SHIELD,
	MAX_BUTTON_BIND
};
enum class GP_AXIS_STATE
{
	AXIS_IDLE = 0,
	AXIS_POSITIVE_DOWN,
	AXIS_POSITIVE_REPEAT,
	AXIS_POSITIVE_RELEASE,
	AXIS_NEGATIVE_DOWN,
	AXIS_NEGATIVE_REPEAT,
	AXIS_NEGATIVE_RELEASE
};

enum class PLAYER
{
	P1 = 0,
	P2,
	P3,
	P4
};

struct Gamepad
{
	// Id's
	_SDL_GameController* id_ptr = nullptr; //SDL_GameController of the Gamepad
	_SDL_Haptic* haptic_ptr = nullptr;

	// Data
	GP_BUTTON_STATE* buttons = nullptr;
	int* axis = nullptr;
	GP_BUTTON_STATE* triggers_state = nullptr;
	GP_AXIS_STATE* multidirection_axis_state = nullptr;
	int index = -1;

	SDL_GameControllerButtonBind* binded_buttons = nullptr;

	bool any_button_down = false;
	bool any_axis_down = false;
	int last_button_pressed = -1;
	int last_axis_pressed = -1;
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(bool start_enabled = true);
	~ModuleInput();

	bool Init(json file) override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseWheel() const
	{
		return mouse_wheel;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	//------------------GAMEPAD FUNCTIONS------------------//
	// Check gamepad button states. Works jut like GetKey
	GP_BUTTON_STATE GetButton(PLAYER p, int id) const
	{
		if (controllers[(int)p].id_ptr != nullptr)
			return controllers[(int)p].buttons[id];
		else
			return BUTTON_IDLE;
	}

	// Check gamepad axis & triggers. returns the integer value of the trigger or axis
	int GetAxis(PLAYER p, int id) const
	{
		if (controllers[(int)p].id_ptr != nullptr)
			return controllers[(int)p].axis[id];
		else
			return 0;
	}


	// Check gamepad button bind. 
	GP_BUTTON_STATE GetButton(PLAYER p, BUTTON_BIND id) const;

	//Bind a button to an action
	void BindButton(PLAYER p, BUTTON_BIND bind, int button_to_bind, int bind_type);

	// PASS A TRIGGER FROM THE ENUM!
	GP_BUTTON_STATE GetTriggerState(PLAYER p, int id) const;

	GP_AXIS_STATE GetLRAxisState(PLAYER p, int id) const;

	//Introduce the controller (linked to the player), the intensity from 0.0 to 1.0 and the duration in miliseconds
	void ShakeController(PLAYER p, float intensity, uint32 length);

	//This function allows you to force a state into a button from a controller, use carefully and only under controlled situations
	void ForceButtonState(PLAYER p, int id, GP_BUTTON_STATE state);

	//Stop the vibration of a controller
	void StopControllerShake(PLAYER p);

	//Button pressed down
	bool AnyButtonDown(PLAYER p) { return controllers[(int)p].any_button_down; }

	//Trigger pressed down
	bool AnyTriggerDown(PLAYER p) { return controllers[(int)p].any_axis_down; }

	// Get the actual binding of a BUTTON_BIND
	int GetBindRealButton(PLAYER p, BUTTON_BIND bind) const;

	//SDL_Rect GetButtonRect(SDL_GameControllerButton button) { return { 0,(int)button * 72, 72,72 }; }

	bool ControllerIsConnected(PLAYER p);

	void SetAxisThreshold(float threshold);

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_wheel;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;

	float axis_threshold = 0.8f;

	//Gamepads Data
	Gamepad controllers[MAX_GAMEPADS] = { nullptr };
	uint index_addition_controllers = 0;
};