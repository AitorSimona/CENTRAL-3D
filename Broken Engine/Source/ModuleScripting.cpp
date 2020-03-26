#include "ModuleScripting.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "Resource.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"

#include "ResourceScript.h"
#include "ComponentScript.h"
#include "ScriptingDebug.h"
#include "ScriptingElements.h"
#include "ScriptingInputs.h"
#include "ScriptingSystems.h"
#include "ScriptVar.h"
#include <iterator>

#include "MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/MathFunc.h"

#include "SDL/include/SDL_keyboard.h"
#include "SDL/include/SDL_mouse.h"

//This include MUST go after Lua includes
//#include "LuaBridge-241/include/LuaBridge.h"
#include "ScriptData.h"
#include "mmgr/mmgr.h"

using namespace Broken;
ModuleScripting::ModuleScripting(bool start_enabled) : Module(start_enabled) {
	name = "Scripting";
}

ModuleScripting::~ModuleScripting() {}

bool ModuleScripting::DoHotReloading() {
	bool ret = true;

	if (App->GetAppState() == AppState::EDITOR) // 	Ask Aitor, only return true when no gameplay is happening	App->scene_intro->playing == false
	{
		// We do the necessary Hot Reloading
		for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it) {
			//Remove the references to the data inside the virtual machine
			(*it)->my_table_class = 0;

			// Copy the necessary data into new instances which will hold the new compiled tables
			ScriptInstance* recompiled_instance = new ScriptInstance;
			recompiled_instance->my_component = (*it)->my_component;
			recompiled_instances.push_back(recompiled_instance);
		}
		// We clean the old instances, close the virtual machine and destroy it
		CleanUpInstances();
		lua_close(L);

		// Create the new Virtual Machine
		L = luaL_newstate();
		luaL_openlibs(L);

		// Acquire all scripts to be compiled (we will compile even scripts which are currently not attached to any GameObject)
		// to check if it still compiles after the change done in a given script which is unknown to us
		std::string extension = "lua";
		std::vector<std::string> files;
		bool can_instantiate = true;

		//App->resources->GetAllFilesWithExtension(extension,files,App->resources->assets_dir); // Here we have to iterate all script resources from that list so we can get the files to recompile

		// Compile all the scripts of the Engine
		for (int i = 0; i < files.size(); ++i) {
			if (JustCompile(files[i]) == false) {
				can_instantiate = false;
				ret = false;
				ENGINE_CONSOLE_LOG("[Warning] Fix all compiler Errors!");
				cannot_start = true;
			}
		}

		if (can_instantiate) {
			// If everything compiled just fine, give the recompiled instances the new version of the script
			for (std::vector<ScriptInstance*>::iterator it = recompiled_instances.begin(); it != recompiled_instances.end(); ++it)
				CompileScriptTableClass((*it));

			for (int i = 0; i < recompiled_instances.size(); ++i)
				class_instances.push_back(recompiled_instances[i]);

			recompiled_instances.clear();
		}

		hot_reloading_waiting = false;
	}
	else {
		hot_reloading_waiting = true;
	}

	if (ret == true)
		cannot_start = false;

	return ret;
}

//This function just compiles a script and checks if it compiled or it prompts the error to console
bool ModuleScripting::JustCompile(std::string absolute_path) {
	bool ret = false;

	//MYTODO: D�dac Commented this so I can try and compile, must uncomment when SCRIPTING class contents are uncommented too
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scripting")

		.beginClass <ScriptingDebug>("Debug")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingElements>("Elements")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingSystems>("Systems")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingInputs>("Inputs")
		.addConstructor<void(*) (void)>()
		.endClass()
		.endNamespace();

	int compiled = luaL_dofile(L, absolute_path.c_str());

	if (compiled == LUA_OK) {
		//We don't need to do nothing here, LOG something at most
		ENGINE_CONSOLE_LOG("Compiled %s successfully!", absolute_path.c_str());
		ret = true;
	}
	else {
		std::string error = lua_tostring(L, -1);
		ENGINE_CONSOLE_LOG("%s", error.data());
		cannot_start = true;
	}

	return ret;
}

void ModuleScripting::CompileScriptTableClass(ScriptInstance* script)
{
	//MYTODO: Didac Commented this so I can try and compile, must uncomment when SCRIPTING class contents are uncommented too
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scripting")

		// ----------------------------------------------------------------------------------
		// SCRIPTING DEBUG
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingDebug>("Debug")
		.addConstructor<void(*) (void)>()
		.addFunction("LOG", &ScriptingDebug::LogFromLua)
		.addFunction("RealDT", &ScriptingDebug::GetRealDT)
		.addFunction("DT", &ScriptingDebug::GetDT)
		.addFunction("GameTime", &ScriptingDebug::GameTime)

		// Maths
		.addFunction("CompareFloats", &ScriptingDebug::FloatNumsAreEqual)
		.addFunction("CompareDoubles", &ScriptingDebug::DoubleNumsAreEqual)
		.endClass()

		// ----------------------------------------------------------------------------------
		// SCRIPTING ELEMENTS
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingElements>("Elements")
		.addConstructor<void(*) (void)>()

		// GameObject-related functions
		.addFunction("FindGameObject", &ScriptingElements::FindGameObject)
		.addFunction("GetGameObjectPos", &ScriptingElements::GetGameObjectPos)
		.addFunction("GetGameObjectPosX", &ScriptingElements::GetGameObjectPosX)
		.addFunction("GetGameObjectPosY", &ScriptingElements::GetGameObjectPosY)
		.addFunction("GetGameObjectPosZ", &ScriptingElements::GetGameObjectPosZ)
		.addFunction("TranslateGameObject", &ScriptingElements::TranslateGameObject)

		.addFunction("GetComponent", &ScriptingElements::GetComponentFromGO)

		// Position
		.addFunction("GetPosition", &ScriptingElements::GetPosition)
		.addFunction("GetPositionX", &ScriptingElements::GetPositionX)
		.addFunction("GetPositionY", &ScriptingElements::GetPositionY)
		.addFunction("GetPositionZ", &ScriptingElements::GetPositionZ)

		.addFunction("Translate", &ScriptingElements::Translate)
		.addFunction("SetPosition", &ScriptingElements::SetPosition)

		// Rotation
		.addFunction("RotateObject", &ScriptingElements::RotateObject)
		.addFunction("SetObjectRotation", &ScriptingElements::SetObjectRotation)
		.addFunction("LookAt", &ScriptingElements::LookAt)

		.addFunction("GetRotation", &ScriptingElements::GetRotation)
		.addFunction("GetRotationX", &ScriptingElements::GetRotationX)
		.addFunction("GetRotationY", &ScriptingElements::GetRotationY)
		.addFunction("GetRotationZ", &ScriptingElements::GetRotationZ)

		// Current Camera Functions
		.addFunction("GetPositionInFrustum", &ScriptingElements::GetPosInFrustum)

		// GetScript functions
		.addFunction("GetScript", &ScriptingElements::GetScript)
		.endClass()

		// ----------------------------------------------------------------------------------
		// SCRIPTING SYSTEMS
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingSystems>("Systems")
		.addConstructor<void(*) (void)>()

		// Physics
		.addFunction("GetMass", &ScriptingSystems::GetMass)
		.addFunction("SetMass", &ScriptingSystems::SetMass)

		.addFunction("GetAngularVelocity", &ScriptingSystems::GetAngularVelocity)
		.addFunction("SetAngularVelocity", &ScriptingSystems::SetAngularVelocity)
		.addFunction("GetLinearVelocity", &ScriptingSystems::GetLinearVelocity)
		.addFunction("SetLinearVelocity", &ScriptingSystems::SetLinearVelocity)

		.addFunction("AddTorque", &ScriptingSystems::AddTorque)
		.addFunction("AddForce", &ScriptingSystems::AddForce)

		.addFunction("UseGravity", &ScriptingSystems::UseGravity)
		.addFunction("SetKinematic", &ScriptingSystems::SetKinematic)

		.addFunction("Move", &ScriptingSystems::Move)

		// Particles
		.addFunction("ActivateParticlesEmission", &ScriptingSystems::ActivateParticlesEmission)
		.addFunction("DeactivateParticlesEmission", &ScriptingSystems::DeactivateParticlesEmission)

		// Audio
		.addFunction("PlayAttackSound", &ScriptingSystems::PlayAttackSound)
		.addFunction("PlayStepSound", &ScriptingSystems::PlayStepSound)
		.addFunction("StopAttackSound", &ScriptingSystems::StopAttackSound)
		.addFunction("StopStepSound", &ScriptingSystems::StopStepSound)
		.addFunction("SetVolume", &ScriptingSystems::SetVolume)

		// Animations
		.addFunction("PlayAnimation", &ScriptingSystems::StartAnimation)
		.addFunction("SetAnimationSpeed", &ScriptingSystems::SetAnimSpeed)
		.addFunction("SetCurrentAnimationSpeed", &ScriptingSystems::SetCurrentAnimSpeed)

		// UI
		.addFunction("SetUIBarPercentage", &ScriptingSystems::SetBarPercentage)
		.addFunction("SetTextinTextComp", &ScriptingSystems::SetUIText)
		.addFunction("SetTextAndNuminTextComp", &ScriptingSystems::SetUITextAndNumber)
		.addFunction("SetNuminTextComp", &ScriptingSystems::SetUITextNumber)

		.endClass()

		// ----------------------------------------------------------------------------------
		// SCRIPTING INPUTS
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingInputs>("Inputs")
		.addConstructor<void(*) (void)>()
		.addFunction("GetKey", &ScriptingInputs::GetKey)
		.addFunction("KeyState", &ScriptingInputs::GetKeyState)

		.addFunction("KeyDown", &ScriptingInputs::IsKeyDown)
		.addFunction("KeyUp", &ScriptingInputs::IsKeyUp)
		.addFunction("KeyRepeat", &ScriptingInputs::IsKeyRepeat)
		.addFunction("KeyIdle", &ScriptingInputs::IsKeyIdle)

		.addFunction("GetMouseButton", &ScriptingInputs::GetMouseButton)
		.addFunction("MouseButtonState", &ScriptingInputs::GetMouseButtonState)

		.addFunction("MouseButtonDown", &ScriptingInputs::IsMouseButtonDown)
		.addFunction("MouseButtonUp", &ScriptingInputs::IsMouseButtonUp)
		.addFunction("MouseButtonRepeat", &ScriptingInputs::IsMouseButtonRepeat)
		.addFunction("MouseButtonIdle", &ScriptingInputs::IsMouseButtonIdle)

		.addFunction("IsGamepadButton", &ScriptingInputs::IsGamepadButton)
		.addFunction("GetControllerButton", &ScriptingInputs::GetControllerButtonFromString)
		.addFunction("GetGamepadButtonState", &ScriptingInputs::GetGamepadButtonState)

		.addFunction("IsJoystickAxis", &ScriptingInputs::IsJoystickAxis)
		.addFunction("IsTriggerState", &ScriptingInputs::IsTriggerState)
		.addFunction("GetControllerAxis", &ScriptingInputs::GetControllerAxisFromString)
		.addFunction("GetAxisState", &ScriptingInputs::GetAxisStateFromString)

		.addFunction("GetAxisRealValue", &ScriptingInputs::GetAxisRealValue)
		.addFunction("GetAxisValue", &ScriptingInputs::GetAxisValue)
		.addFunction("ShakeController", &ScriptingInputs::ShakeController)
		.addFunction("StopControllerShake", &ScriptingInputs::StopControllerShake)
		.endClass()
		.endNamespace();

	if (L != nullptr) {
		// Compile the file and run it, we're gonna optimize this to just compile the function the script contains to library later.
		int compiled = luaL_dofile(L, script->my_component->script->absolute_path.c_str());

		if (compiled == LUA_OK) {
			// Get the function to instantiate the lua table (used as a class as known in C++)
			std::string get_function = "GetTable" + script->my_component->script->script_name;
			App->fs->SplitFilePath(get_function.data(), nullptr, &get_function, nullptr);
			luabridge::LuaRef ScriptGetTable = luabridge::getGlobal(L, get_function.c_str());

			if (!ScriptGetTable.isNil()) {
				luabridge::LuaRef table(ScriptGetTable());

				script->my_table_class = table;
				FillScriptInstanceComponentVars(script);
			}
		}
		else {
			std::string error = lua_tostring(L, -1);
			ENGINE_CONSOLE_LOG("%s", error.data());
			cannot_start = true;
		}
	}
}

void ModuleScripting::SendScriptToModule(ComponentScript* script_component) {
	ScriptInstance* s_instance = new ScriptInstance;
	s_instance->my_component = script_component;

	class_instances.push_back(s_instance);
	JustCompile(script_component->script->absolute_path);
	CompileScriptTableClass(s_instance); // Compile so we can give the instance its table/class reference
}

// Fill the ScriptVars of the component associated with this script
void ModuleScripting::FillScriptInstanceComponentVars(ScriptInstance* script) {

	// Reset the type of all the variables
	for (int i = 0; i < script->my_component->script_variables.size(); ++i)
		script->my_component->script_variables[i].type = VarType::NONE;

	for (luabridge::Iterator iterator(script->my_table_class); !iterator.isNil(); ++iterator) {
		// Declare necessary vars for intialization & get variable name
		VarType variable_type = VarType::NONE;
		std::string str = (*iterator).first.tostring();
		ScriptVar variable;
		std::string var_value;

		// Fill values --> first == .key () and second == .value() in Lua
		if ((*iterator).second.isNumber()) {
			variable_type = VarType::DOUBLE;
			var_value = (*iterator).second.tostring();
			double val = std::stod(var_value);
			variable = ScriptVar(val);
		}
		else if ((*iterator).second.isString()) {
			variable_type = VarType::STRING;
			var_value = (*iterator).second.tostring();
			variable = ScriptVar(var_value.data());
		}
		else if ((*iterator).second.isBool()) {
			variable_type = VarType::BOOLEAN;
			var_value = (*iterator).second.tostring();
			bool val = false;
			std::string tru = "true";
			if (!var_value.compare(tru)) {
				val = true;
			}
			variable = ScriptVar(val);
		}
		else continue;

		// Assign name to variable and push it if is compatible
		variable.name = str;
		int variable_index = script->my_component->ScriptVarAlreadyInComponent(variable.name);
		if (variable_type != VarType::NONE) {

			//If the var was already on the component (in case of hot reloading)
			if (variable_index > -1)
			{
				// Check that the variable is still of the same type before changing any value
				if (variable.type == script->my_component->script_variables[variable_index].type) {
					script->my_component->script_variables[variable_index].editor_value = variable.editor_value;
				}
				// The variable changed its type
				else
				{
					script->my_component->script_variables[variable_index] = variable;
				}
			}
			else {
				script->my_component->script_variables.push_back(variable);
			}
		}
	}

	// Erase from memory variables deleted in hot reloading (those that are still type NONE)
	for (int i = 0; i < script->my_component->script_variables.size(); ++i) {
		if (script->my_component->script_variables[i].type == VarType::NONE) {
			std::vector<ScriptVar>::iterator var_it = std::find(script->my_component->script_variables.begin(), script->my_component->script_variables.end(), script->my_component->script_variables[i]);
			script->my_component->script_variables.erase(var_it);
		}
	}
	//Fill Component Functions
	FillScriptInstanceComponentFuncs(script);
}

void ModuleScripting::FillScriptInstanceComponentFuncs(ScriptInstance* script)
{
	uint functions_num = 0;
	for (luabridge::Iterator iterator(script->my_table_class); !iterator.isNil(); ++iterator) {
		ScriptFunc function;

		if ((*iterator).second.isFunction()) {
			function.name = (*iterator).first.tostring();

			if (!function.name.compare("Awake") || !function.name.compare("Start") || !function.name.compare("Update"))
				continue;

			functions_num++;
		}
		else continue;

		if (!function.name.compare("null_function"))
		{
			//Something went wrong, LOG Component name & GameObject affected
			std::string aux_str = script->my_component->GetContainerGameObject()->GetName();
			ENGINE_CONSOLE_LOG("Tried to add a ''null_function'' to script component: %s of GameObject: %s", script->my_component->script_name.c_str(), aux_str.c_str());
		}
		else if (script->my_component->ScriptFuncAlreadyInComponent(function.name) == false)
		{
			//Function is not yet in component list. Add it!
			script->my_component->script_functions.push_back(function);
		}
	}

	if (functions_num < script->my_component->script_functions.size()) // Functions were deleted!
	{
		//delete all functions and reload, a bit costly but easy & reliable
		script->my_component->script_functions.clear();
		FillScriptInstanceComponentFuncs(script);
	}
}

void ModuleScripting::DeleteScriptInstanceWithParentComponent(ComponentScript* script_component) {
	for (int i = 0; i < class_instances.size(); ++i) {
		if (class_instances[i] != nullptr && class_instances[i]->my_component == script_component) {
			//delete class_instances[i];
			class_instances.erase(class_instances.begin() + i);
		}
	}
}

void ModuleScripting::NullifyScriptInstanceWithParentComponent(ComponentScript* script_component)
{
	for (int i = 0; i < class_instances.size(); ++i)
	{
		if (class_instances[i] != nullptr && class_instances[i]->my_component == script_component)
		{
			class_instances[i]->script_is_null = true;

			ENGINE_AND_SYSTEM_CONSOLE_LOG("Lua Resource for component %s (lua script component) is nullptr!",class_instances[i]->my_component->script_name.c_str());
		}
	}
}

void ModuleScripting::NotifyHotReloading()
{
	hot_reloading_waiting = true;
}

bool ModuleScripting::CheckEverythingCompiles()
{
	for (int i = 0; i < class_instances.size(); ++i)
	{
		if ( !class_instances[i]->script_is_null &&  !JustCompile(class_instances[i]->my_component->script->absolute_path))
		{
			cannot_start = true;
			ENGINE_AND_SYSTEM_CONSOLE_LOG("Cannot start since there are compilation errors in the LUA scripts!");
			return false;
		}
	}

	return true;
}

//Calls a determined function from a lua script
void ModuleScripting::CallbackScriptFunction(ComponentScript* script_component, const ScriptFunc& function_to_call)
{
	ScriptInstance* script = GetScriptInstanceFromComponent(script_component);

	std::string aux_str = function_to_call.name;
	if (script != nullptr)
	{
		if (App->GetAppState() == AppState::PLAY)
		{
			script->my_table_class[aux_str.c_str()](); // call to Lua to execute the given function
			ENGINE_CONSOLE_LOG("Callback of function %s", aux_str.c_str());
		}
	}
	else
	{
		ENGINE_CONSOLE_LOG("Can't callback %s since component has a null script instance", aux_str.c_str());
	}
}


bool ModuleScripting::Init(json& file) {
	// Create the Virtual Machine
	L = luaL_newstate();
	luaL_openlibs(L);

	return true;
}

bool ModuleScripting::Start() {
	return true;
}

bool ModuleScripting::CleanUp() {
	CleanUpInstances();

	return true;
}

update_status ModuleScripting::Update(float realDT) {
	// If a script was changed during runtime, hot reload
	if (App->GetAppState() == AppState::EDITOR && hot_reloading_waiting) // Ask Aitor if this is correct (condition should return true only when no gameplay is being played)
		DoHotReloading();

	if(App->GetAppState() != AppState::PLAY)
	{
		previous_AppState = (_AppState)App->GetAppState();

		for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		{
			(*it)->awoken = false;
			(*it)->started = false;
		}
	}

	// Carles to Didac
	// 1. You can use the "IsWhatever" functions of App to check the current game state.
	// 2. "App->IsGameFirstFrame()" marks the first frame a GameUpdate() will happen, if you want to do anything right before the game plays in preparation
	// 3. Referring to your previous code, you expected DoHotReloading() to NOT run if the game is playing, I put the condition accordingly "!IsGamePlaying()"

	//TEST FUNCTION DEFINETIVELY SHOULD NOT BE HERE
	//MYTODO: Didac PLEAse didac look into this why did you do this?
	/*if (App->scene_intro->selected_go != nullptr && App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
		GameObject* returned = GOFunctions::InstantiateGameObject(App->scene_intro->selected_go);*/
	return UPDATE_CONTINUE;
}

update_status ModuleScripting::GameUpdate(float gameDT)
{
	if (cannot_start == false && App->GetAppState() == AppState::PLAY)
	{
		const uint origSize = class_instances.size();	// This avoids messing the iteration with newly Instantiated scripts
		for (uint i = 0; i < origSize; ++i) {
			current_script = class_instances[i];
			if (current_script->script_is_null == false)
			{
				std::string aux_str = "null";
				for (std::vector<ScriptVar>::iterator it = current_script->my_component->script_variables.begin(); it != current_script->my_component->script_variables.end(); ++it)
				{
					if ((*it).changed_value) {
						switch ((*it).type) {
						case VarType::DOUBLE:
							current_script->my_table_class[(*it).name.c_str()] = (*it).editor_value.as_double;
							break;
						case VarType::STRING:
							aux_str = (*it).editor_value.as_string;
							current_script->my_table_class[(*it).name.c_str()] = aux_str.c_str();
							break;
						case VarType::BOOLEAN:
							current_script->my_table_class[(*it).name.c_str()] = (*it).editor_value.as_boolean;
							break;
						}
						(*it).changed_value = false;
					}
				}

				if (current_script->awoken == false)
				{
					current_script->my_table_class["Awake"]();	// Awake is done first, regardless of the script being active or not
					current_script->awoken = true;
				}
				else if (current_script->my_component->GetActive()) //Check if the script instance and it's object holding it are active
				{
					GameObject* GO = current_script->my_component->GetContainerGameObject();

					if (GO->GetActive()) // Even if the component script is active, if its gameobject its not, we won't update
					{
						if (current_script->started == false)
						{
							current_script->my_table_class["Start"]();	// Start is done only once for the first time the script is active
							current_script->started = true;
						}
						else
						{
							current_script->my_table_class["Update"]();	// Update is done on every iteration of the script as long as it remains active
							FillScriptInstanceComponentVars(current_script); // Show variables at runtime
						}
					}
				}
			}
		}
	}
	else if (cannot_start == true && App->GetAppState() == AppState::PLAY && previous_AppState == AppState::EDITOR)
	{
		CheckEverythingCompiles();
	}

	previous_AppState = (_AppState)App->GetAppState();
	return UPDATE_CONTINUE;
}

void ModuleScripting::CleanUpInstances() {
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it) {
		if ((*it) != nullptr)
			delete (*it);
	}

	class_instances.clear();
}

//Return a pointer to a Script instance that is attached to the component script
ScriptInstance* ModuleScripting::GetScriptInstanceFromComponent(ComponentScript* component_script)
{
	ScriptInstance* ret = nullptr;

	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
	{
		if ((*it)->my_component == component_script)
		{
			ret = (*it);
			break;
		}
	}

	return ret;
}

bool ModuleScripting::Stop() {
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		(*it)->awoken = (*it)->started = false;

	return true;
}
