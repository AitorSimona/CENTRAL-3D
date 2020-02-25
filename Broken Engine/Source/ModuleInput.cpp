#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "Importer.h"

#include "mmgr/mmgr.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	
	//ALLOC memory for each gamepad buttons & Axis
	for (int i = 0; i < MAX_GAMEPADS; ++i)
	{
		controllers[i].buttons = new GP_BUTTON_STATE[SDL_CONTROLLER_BUTTON_MAX];
		memset(controllers[i].buttons, BUTTON_IDLE, sizeof(GP_BUTTON_STATE) * SDL_CONTROLLER_BUTTON_MAX);

		controllers[i].axis = new int[SDL_CONTROLLER_AXIS_MAX];
		memset(controllers[i].axis, 0, sizeof(int) * SDL_CONTROLLER_AXIS_MAX);

		//Allocate space for button binders
		controllers[i].binded_buttons = new SDL_GameControllerButtonBind[uint(BUTTON_BIND::MAX_BUTTON_BIND)];
		memset(controllers[i].binded_buttons, SDL_CONTROLLER_BUTTON_MAX, sizeof(SDL_GameControllerButtonBind) * (int)BUTTON_BIND::MAX_BUTTON_BIND);

		controllers[i].triggers_state = new GP_BUTTON_STATE[2]; // There are only 2 Triggers in a gamepad
		memset(controllers[i].triggers_state, BUTTON_IDLE, sizeof(GP_BUTTON_STATE) * 2);

		controllers[i].multidirection_axis_state = new GP_AXIS_STATE[4]; // There are only 4 multidirectional axis in a gamepad, left X & Y, Right X & Y
		memset(controllers[i].multidirection_axis_state, (int)GP_AXIS_STATE::AXIS_IDLE, sizeof(GP_AXIS_STATE) * 4);
	}
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(json file)
{
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	//Gamepad Inits
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

	if (SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
	{
		ENGINE_AND_SYSTEM_CONSOLE_LOG("SDL_HAPTICS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_GameControllerEventState(SDL_ENABLE);

	if (SDL_GameControllerEventState(SDL_QUERY) != 1)
	{
		ENGINE_AND_SYSTEM_CONSOLE_LOG("SDL_GAME_CONTROLLER_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_wheel = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		// --- NEED to tell Imgui about the INPUT!! ---
		App->gui->HandleInput(&e);

		std::string DroppedFile_path; // Moved string declaration here because compiler complained of it being ommited due to scoping inside the switch
		Importer::ImportData IData("null_path");
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_wheel = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					App->window->SetWindowWidth(e.window.data1);
					App->window->SetWindowHeight(e.window.data2);
					App->window->UpdateWindowSize();
				}
			}
			break;

			case SDL_DROPFILE:
				// --- Call Resource Manager on file drop ---

				DroppedFile_path = e.drop.file;

				IData.path = DroppedFile_path.c_str();
				//IData(DroppedFile_path.c_str());
				IData.dropped = true;
				App->resources->ImportAssets(IData);

				SDL_free((char*)DroppedFile_path.data());
			break;

			//When a controller is plugged in
			case SDL_CONTROLLERDEVICEADDED:
			{
				int n_joys = SDL_NumJoysticks();

				if (SDL_IsGameController(n_joys - 1))
				{
					for (int i = 0; i < n_joys; ++i)
					{
						if (controllers[i].id_ptr == nullptr) // If there isn't a gamepad connected  already
						{
							if (controllers[i].index == -1) //First time a gamepad has been connected
							{
								controllers[i].id_ptr = SDL_GameControllerOpen(index_addition_controllers);

								controllers[i].haptic_ptr = SDL_HapticOpen(index_addition_controllers);
								SDL_HapticRumbleInit(controllers[i].haptic_ptr);

								controllers[i].index = index_addition_controllers;
								ENGINE_CONSOLE_LOG("Gamepad %d was Connected for the 1st Time!", i+1);
								//LoadConfigBinding((PLAYER)controllers[i].index);
							}
							else    //The gamepad was disconnected at some point and is now being reconnected
							{
								controllers[i].id_ptr = SDL_GameControllerOpen(controllers[i].index);
								ENGINE_CONSOLE_LOG("Gamepad %d was reconnected!", i);
							}


							// This index will assign the proper index for a gamapd that has been connected once
							// in case it is disconnected and connected again it will use the value of the var 
							// at the moment of opening the gamepad
							if (index_addition_controllers < MAX_GAMEPADS - 1)
								index_addition_controllers++;

							break;
						}
					}
				}
			}
			break;
		}
	}


	//Check Gamepads
	for (int i = 0; i < MAX_GAMEPADS; i++)
	{
		controllers[i].any_button_down = false;
		controllers[i].any_axis_down = false;
		if (SDL_GameControllerGetAttached(controllers[i].id_ptr) == SDL_TRUE) // If it is opened correctly
		{
			//Check all button states basically, ame process as keyboard but with gamepads
			for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j)
			{
				if (SDL_GameControllerGetButton(controllers[i].id_ptr, (SDL_GameControllerButton)j) == 1)
				{
					if (controllers[i].buttons[j] == BUTTON_IDLE)
					{
						controllers[i].buttons[j] = BUTTON_DOWN;
						controllers[i].last_button_pressed = j;
						controllers[i].any_button_down = true;
					}
					else
						controllers[i].buttons[j] = BUTTON_REPEAT;
				}
				else
				{
					if (controllers[i].buttons[j] == BUTTON_REPEAT || controllers[i].buttons[j] == BUTTON_DOWN)
						controllers[i].buttons[j] = BUTTON_UP;
					else
						controllers[i].buttons[j] = BUTTON_IDLE;
				}
			}

			//----------------------GAMEPAD_UPDATE----------------------//
			// Check all Axis & Triggers
			for (int j = 0; j < SDL_CONTROLLER_AXIS_MAX; ++j)
			{
				controllers[i].axis[j] = SDL_GameControllerGetAxis(controllers[i].id_ptr, (SDL_GameControllerAxis)j);
			}

			// Check Triggers to work as states
			for (int j = 0; j < 2; ++j)
			{
				if (SDL_GameControllerGetAxis(controllers[i].id_ptr, SDL_GameControllerAxis(j + (int)SDL_CONTROLLER_AXIS_TRIGGERLEFT)) > 0.8f * AXISMAX)
				{
					if (controllers[i].triggers_state[j] == BUTTON_IDLE)
					{
						controllers[i].triggers_state[j] = BUTTON_DOWN;
						controllers[i].last_axis_pressed = j + (int)SDL_CONTROLLER_AXIS_TRIGGERLEFT;
						controllers[i].any_axis_down = true;
					}
					else
						controllers[i].triggers_state[j] = BUTTON_REPEAT;
				}
				else
				{
					if (controllers[i].triggers_state[j] == BUTTON_REPEAT || controllers[i].triggers_state[j] == BUTTON_DOWN)
						controllers[i].triggers_state[j] = BUTTON_UP;
					else
						controllers[i].triggers_state[j] = BUTTON_IDLE;
				}
			}

			// Check Multidirection axis to work as states
			for (int j = 0; j < 4; ++j)
			{
				if (SDL_GameControllerGetAxis(controllers[i].id_ptr, SDL_GameControllerAxis(j)) > 0.8f * AXISMAX) //Stick all the way to the positive
				{
					if (controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_IDLE ||
						IN_RANGE(controllers[i].multidirection_axis_state[j], GP_AXIS_STATE::AXIS_NEGATIVE_DOWN, GP_AXIS_STATE::AXIS_NEGATIVE_REPEAT))
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_POSITIVE_DOWN;
					else
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_POSITIVE_REPEAT;
				}
				else if (IN_RANGE(controllers[i].multidirection_axis_state[j], GP_AXIS_STATE::AXIS_POSITIVE_DOWN, GP_AXIS_STATE::AXIS_POSITIVE_RELEASE))
				{
					if (controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_POSITIVE_REPEAT || controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_POSITIVE_DOWN)
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_POSITIVE_RELEASE;
					else
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_IDLE;
				}
				else if (SDL_GameControllerGetAxis(controllers[i].id_ptr, SDL_GameControllerAxis(j)) < -(0.8f * AXISMAX))// Stick All the way negative
				{
					if (controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_IDLE ||
						IN_RANGE(controllers[i].multidirection_axis_state[j], GP_AXIS_STATE::AXIS_POSITIVE_DOWN, GP_AXIS_STATE::AXIS_POSITIVE_REPEAT))
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_NEGATIVE_DOWN;
					else
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_NEGATIVE_REPEAT;
				}
				else if (IN_RANGE(controllers[i].multidirection_axis_state[j], GP_AXIS_STATE::AXIS_NEGATIVE_DOWN, GP_AXIS_STATE::AXIS_NEGATIVE_RELEASE)) // Stick neither all the way negative or positive
				{
					if (controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_NEGATIVE_REPEAT || controllers[i].multidirection_axis_state[j] == GP_AXIS_STATE::AXIS_NEGATIVE_DOWN)
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_NEGATIVE_RELEASE;
					else
						controllers[i].multidirection_axis_state[j] = GP_AXIS_STATE::AXIS_IDLE;
				}
			}


		}
		else if (controllers[i].id_ptr != nullptr) // Controller disattached, close (and set to nullptr)
		{
			SDL_GameControllerClose(controllers[i].id_ptr);
			controllers[i].id_ptr = nullptr;

			/*SDL_HapticClose(controllers[i].haptic_ptr);
			controllers[i].haptic_ptr = nullptr;*/
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Quitting SDL input event subsystem");

	for (int i = 0; i < MAX_GAMEPADS; ++i)
	{
		if (controllers[i].id_ptr != nullptr)
		{
			if (SDL_GameControllerGetAttached(controllers[i].id_ptr))
				//SDL_GameControllerClose(controllers[i].id_ptr); //seems to have a very weird bug where it crashes the app

				controllers[i].id_ptr = nullptr;
		}

		if (controllers[i].haptic_ptr != nullptr)
		{
			SDL_HapticClose(controllers[i].haptic_ptr);
			controllers[i].haptic_ptr = nullptr;
		}

		delete[] controllers[i].buttons;
		delete[] controllers[i].axis;
		delete[] controllers[i].binded_buttons;

		/*controllers[i].buttons = nullptr;
		controllers[i].axis = nullptr;
		controllers[i].binded_buttons = nullptr;*/
		delete[] controllers[i].triggers_state;
		delete[] controllers[i].multidirection_axis_state;
	}

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_QuitSubSystem(SDL_INIT_HAPTIC);

	return true;
}

GP_BUTTON_STATE ModuleInput::GetTriggerState(PLAYER p, int id) const
{
	if (id == SDL_CONTROLLER_AXIS_TRIGGERLEFT || SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
	{
		if (controllers[(int)p].id_ptr != nullptr)
			return controllers[(int)p].triggers_state[(id - (int)SDL_CONTROLLER_AXIS_TRIGGERLEFT)];
	}
	else
		return BUTTON_IDLE;
}

GP_AXIS_STATE ModuleInput::GetLRAxisState(PLAYER p, int id) const
{
	if (IN_RANGE(id, (int)SDL_CONTROLLER_AXIS_LEFTX, (int)SDL_CONTROLLER_AXIS_RIGHTY))
	{
		if (controllers[(int)p].id_ptr != nullptr)
		{
			return controllers[(int)p].multidirection_axis_state[id - (int)SDL_CONTROLLER_AXIS_LEFTX];
		}
	}
	else
		return GP_AXIS_STATE::AXIS_IDLE;
}

GP_BUTTON_STATE ModuleInput::GetButton(PLAYER p, BUTTON_BIND id) const
{
	if (controllers[(int)p].id_ptr != nullptr && controllers[(int)p].binded_buttons[(int)id].bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
		return controllers[(int)p].buttons[controllers[(int)p].binded_buttons[(int)id].value.button];
	else if (controllers[(int)p].id_ptr != nullptr && controllers[(int)p].binded_buttons[(int)id].bindType == SDL_CONTROLLER_BINDTYPE_HAT)
		return controllers[(int)p].buttons[controllers[(int)p].binded_buttons[(int)id].value.button];
	else if (controllers[(int)p].id_ptr != nullptr && controllers[(int)p].binded_buttons[(int)id].bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
	{
		if (controllers[(int)p].binded_buttons[(int)id].value.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
			controllers[(int)p].binded_buttons[(int)id].value.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
		{
			return GetTriggerState(p, controllers[(int)p].binded_buttons[(int)id].value.axis);
		}
	}
	else
		return BUTTON_IDLE;
}

void ModuleInput::BindButton(PLAYER p, BUTTON_BIND bind, int button_to_bind, int bind_type)
{
	if ((SDL_GameControllerBindType)bind_type == SDL_CONTROLLER_BINDTYPE_AXIS)
	{
		if ((SDL_GameControllerAxis)button_to_bind == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
		{
			SDL_GameControllerGetBindForAxis(controllers[(int)p].id_ptr, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			controllers[(int)p].binded_buttons[(int)bind].value.axis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
			controllers[(int)p].binded_buttons[(int)bind].bindType = SDL_CONTROLLER_BINDTYPE_AXIS;
		}
		else if ((SDL_GameControllerAxis)button_to_bind == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
		{
			SDL_GameControllerGetBindForAxis(controllers[(int)p].id_ptr, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			controllers[(int)p].binded_buttons[(int)bind].value.axis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
			controllers[(int)p].binded_buttons[(int)bind].bindType = SDL_CONTROLLER_BINDTYPE_AXIS;
		}
	}
	else
	{
		controllers[(int)p].binded_buttons[(int)bind] = SDL_GameControllerGetBindForButton(controllers[(int)p].id_ptr, (SDL_GameControllerButton)button_to_bind);
		controllers[(int)p].binded_buttons[(int)bind].value.button = (SDL_GameControllerButton)button_to_bind;
	}
}

void ModuleInput::ShakeController(PLAYER p, float intensity, uint32 length)
{
	if (controllers[(int)p].id_ptr != nullptr)
		SDL_HapticRumblePlay(controllers[(int)p].haptic_ptr, intensity, length);
}

void ModuleInput::ForceButtonState(PLAYER p, int id, GP_BUTTON_STATE state)
{
	if (controllers[(int)p].id_ptr != nullptr)
		controllers[(int)p].buttons[id] = state;
}

void ModuleInput::StopControllerShake(PLAYER p)
{
	if (controllers[(int)p].id_ptr != nullptr)
		SDL_HapticRumbleStop(controllers[(int)p].haptic_ptr);
}

int ModuleInput::GetBindRealButton(PLAYER p, BUTTON_BIND bind) const
{
	if (controllers[(int)p].binded_buttons[(int)bind].bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
		return int(controllers[(int)p].binded_buttons[(int)bind].value.button);
	else if (controllers[(int)p].binded_buttons[(int)bind].bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
		return (int(controllers[(int)p].binded_buttons[(int)bind].value.axis) + SDL_CONTROLLER_BUTTON_MAX);

	return int(controllers[(int)p].binded_buttons[(int)bind].value.button);
}

bool ModuleInput::ControllerIsConnected(PLAYER p)
{
	if (controllers[(int)p].id_ptr == nullptr)
		return false;
	else
		return true;
}