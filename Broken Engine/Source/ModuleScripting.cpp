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
#include "ModuleEventManager.h"

#include "ResourceScript.h"
#include "ComponentScript.h"
#include "ScriptingSystem.h"
#include "ScriptingTransform.h"
#include "ScriptingPhysics.h"
#include "ScriptingInputs.h"
#include "ScriptingAnimations.h"
#include "ScriptingAudio.h"
#include "ScriptingCamera.h"
#include "ScriptingGameobject.h"
#include "ScriptingParticles.h"
#include "ScriptingInterface.h"
#include "ScriptingScenes.h"
#include "ScriptingNavigation.h"
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

	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scripting")

		.beginClass <ScriptingSystem>("System")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingTransform>("Transform")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingGameobject>("GameObject")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingPhysics>("Physics")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingParticles>("Particles")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingAudio>("Audio")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingAnimations>("Animations")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingInterface>("Interface")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingScenes>("Scenes")
		.addConstructor<void(*) (void)>()
		.endClass()

		.beginClass <ScriptingNavigation>("Navigation")
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
		// SYSTEM
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingSystem>("System")
		.addConstructor<void(*) (void)>()
		.addFunction("LOG", &ScriptingSystem::LogFromLua)
		.addFunction("RealDT", &ScriptingSystem::GetRealDT)
		.addFunction("DT", &ScriptingSystem::GetDT)
		.addFunction("GameTime", &ScriptingSystem::GameTime)
		.addFunction("PauseGame", &ScriptingSystem::PauseGame)
		.addFunction("ResumeGame", &ScriptingSystem::ResumeGame)
		.addFunction("GetDebuggingPath", &ScriptingSystem::GetDebuggingPath)

		// Maths
		.addFunction("CompareFloats", &ScriptingSystem::FloatNumsAreEqual)
		.addFunction("CompareDoubles", &ScriptingSystem::DoubleNumsAreEqual)
		.addFunction("RandomNumber", &ScriptingSystem::RandomNumber)
		.addFunction("RandomNumberInRange", &ScriptingSystem::RandomNumberInRange)
		.addFunction("RandomNumberList", &ScriptingSystem::RandomNumberList)
		.endClass()

		// ----------------------------------------------------------------------------------
		// TRANSFORM
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingTransform>("Transform")
		.addConstructor<void(*) (void)>()

		// Position
		.addFunction("GetPosition", &ScriptingTransform::GetPosition)
		.addFunction("Translate", &ScriptingTransform::Translate)
		.addFunction("SetPosition", &ScriptingTransform::SetPosition)

		// Rotation
		.addFunction("GetRotation", &ScriptingTransform::GetRotation)
		.addFunction("RotateObject", &ScriptingTransform::RotateObject)
		.addFunction("SetObjectRotation", &ScriptingTransform::SetObjectRotation)
		.addFunction("LookAt", &ScriptingTransform::LookAt)
		.endClass()

		// ----------------------------------------------------------------------------------
		// GAMEOBJECT
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingGameobject>("GameObject")
		.addConstructor<void(*) (void)>()

		.addFunction("FindGameObject", &ScriptingGameobject::FindGameObject)
		.addFunction("GetMyUID", &ScriptingGameobject::GetMyUID)
		.addFunction("GetParent", &ScriptingGameobject::GetScriptGOParent)
		.addFunction("GetGameObjectParent", &ScriptingGameobject::GetGOParentFromUID)
		.addFunction("DestroyGameObject", &ScriptingGameobject::DestroyGOFromScript)
		.addFunction("SetActiveGameObject", &ScriptingGameobject::SetActiveGameObject)

		.addFunction("GetMyLayer", &ScriptingGameobject::GetMyLayer)
		.addFunction("GetLayerByID", &ScriptingGameobject::GetLayerByID)
		.addFunction("GetComponent", &ScriptingGameobject::GetComponentFromGO)
		.addFunction("GetScript", &ScriptingGameobject::GetScript)
		.endClass()

		// ----------------------------------------------------------------------------------
		// CAMERA
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingCamera>("Camera")
		.addConstructor<void(*) (void)>()
		.addFunction("GetPositionInFrustum", &ScriptingCamera::GetPosInFrustum)
		.addFunction("GetFrustumPlanesIntersection", &ScriptingCamera::GetFrustumPlanesIntersection)
		.addFunction("WorldToScreen", &ScriptingCamera::WorldToScreen)
		.addFunction("ScreenToWorld", &ScriptingCamera::ScreenToWorld)
		.endClass()

		// ----------------------------------------------------------------------------------
		// PHYSICS
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingPhysics>("Physics")
		.addConstructor<void(*) (void)>()

		.addFunction("GetMass", &ScriptingPhysics::GetMass)
		.addFunction("SetMass", &ScriptingPhysics::SetMass)

		.addFunction("GetAngularVelocity", &ScriptingPhysics::GetAngularVelocity)
		.addFunction("SetAngularVelocity", &ScriptingPhysics::SetAngularVelocity)
		.addFunction("GetLinearVelocity", &ScriptingPhysics::GetLinearVelocity)
		.addFunction("SetLinearVelocity", &ScriptingPhysics::SetLinearVelocity)

		.addFunction("AddTorque", &ScriptingPhysics::AddTorque)
		.addFunction("AddForce", &ScriptingPhysics::AddForce)
		.addFunction("UseGravity", &ScriptingPhysics::UseGravity)
		.addFunction("SetKinematic", &ScriptingPhysics::SetKinematic)

		.addFunction("OnTriggerEnter", &ScriptingPhysics::OnTriggerEnter)
		.addFunction("OnTriggerStay", &ScriptingPhysics::OnTriggerStay)
		.addFunction("OnTriggerExit", &ScriptingPhysics::OnTriggerExit)

		.addFunction("OnCollisionEnter", &ScriptingPhysics::OnCollisionEnter)
		.addFunction("OnCollisionStay", &ScriptingPhysics::OnCollisionStay)
		.addFunction("OnCollisionExit", &ScriptingPhysics::OnCollisionExit)

		.addFunction("Move", &ScriptingPhysics::Move)
		.addFunction("GetCharacterPosition", &ScriptingPhysics::GetCharacterPosition)
		.addFunction("SetCharacterPosition", &ScriptingPhysics::SetCharacterPosition)
		.addFunction("GetCharacterUpDirection", &ScriptingPhysics::GetCharacterUpDirection)
		.addFunction("SetCharacterUpDirection", &ScriptingPhysics::SetCharacterUpDirection)

		.addFunction("OverlapSphere", &ScriptingPhysics::OverlapSphere)
		.addFunction("Raycast", &ScriptingPhysics::Raycast)
		.endClass()

		// ----------------------------------------------------------------------------------
		// PARTICLES
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingParticles>("Particles")
		.addConstructor<void(*) (void)>()

		.addFunction("ActivateParticlesEmission", &ScriptingParticles::ActivateParticleEmitter)
		.addFunction("DeactivateParticlesEmission", &ScriptingParticles::DeactivateParticleEmitter)
		
		.addFunction("PlayParticleEmitter", &ScriptingParticles::PlayParticleEmitter)
		.addFunction("StopParticleEmitter", &ScriptingParticles::StopParticleEmitter)
		.addFunction("SetEmissionRate", &ScriptingParticles::SetEmissionRateFromScript)
		.addFunction("SetParticlesPerCreation", &ScriptingParticles::SetParticlesPerCreationFromScript)

		.addFunction("SetExternalAcceleration", &ScriptingParticles::SetParticleAcceleration)
		.addFunction("SetParticlesVelocity", &ScriptingParticles::SetParticleVelocityFromScript)
		.addFunction("SetRandomParticlesVelocity", &ScriptingParticles::SetRandomParticleVelocity)

		.addFunction("SetParticlesLooping", &ScriptingParticles::SetParticleLooping)
		.addFunction("SetParticlesDuration", &ScriptingParticles::SetParticleDuration)
		.addFunction("SetParticlesLifeTime", &ScriptingParticles::SetParticleLifeTime)

		.addFunction("SetParticlesScale", &ScriptingParticles::SetParticleScaleFromScript)
		.addFunction("SetRandomParticlesScale", &ScriptingParticles::SetRandomParticleScale)
		.endClass()

		// ----------------------------------------------------------------------------------
		// AUDIO
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingAudio>("Audio")
		.addConstructor<void(*) (void)>()

		.addFunction("SetVolume", &ScriptingAudio::SetVolume)
		.addFunction("PlayAudioEvent", &ScriptingAudio::PlayAudioEvent)
		.addFunction("StopAudioEvent", &ScriptingAudio::StopAudioEvent)
		.addFunction("PauseAudioEvent", &ScriptingAudio::PauseAudioEvent)
		.addFunction("ResumeAudioEvent", &ScriptingAudio::ResumeAudioEvent)
		.endClass()

		// ----------------------------------------------------------------------------------
		// ANIMATIONS
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingAnimations>("Animations")
		.addConstructor<void(*) (void)>()

		.addFunction("PlayAnimation", &ScriptingAnimations::StartAnimation)
		.addFunction("SetAnimationSpeed", &ScriptingAnimations::SetAnimSpeed)
		.addFunction("SetCurrentAnimationSpeed", &ScriptingAnimations::SetCurrentAnimSpeed)
		.addFunction("SetBlendTime", &ScriptingAnimations::SetBlendTime)
		.endClass()

		// ----------------------------------------------------------------------------------
		// USER INTERFACE
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingInterface>("Interface")
		.addConstructor<void(*) (void)>()

		.addFunction("MakeElementVisible", &ScriptingInterface::MakeUIComponentVisible)
		.addFunction("MakeElementInvisible", &ScriptingInterface::MakeUIComponentInvisible)

		.addFunction("SetUIBarPercentage", &ScriptingInterface::SetBarPercentage)
		.addFunction("SetUICircularBarPercentage", &ScriptingInterface::SetCircularBarPercentage)
		.addFunction("SetText", &ScriptingInterface::SetUIText)
		.addFunction("SetTextAndNumber", &ScriptingInterface::SetUITextAndNumber)
		.addFunction("SetTextNumber", &ScriptingInterface::SetUITextNumber)
		.endClass()

		// ----------------------------------------------------------------------------------
		// SCENES
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingScenes>("Scenes")
		.addConstructor<void(*) (void)>()

		.addFunction("LoadScene", &ScriptingScenes::LoadSceneFromScript)
		.addFunction("QuitGame", &ScriptingScenes::QuitGame)
		.addFunction("Instantiate", &ScriptingScenes::Instantiate)
		.endClass()

		// ----------------------------------------------------------------------------------
		// NAVIGATION
		// ----------------------------------------------------------------------------------
		.beginClass <ScriptingNavigation>("Navigation")
		.addConstructor<void(*) (void)>()

		.addFunction("AllAreas", &ScriptingNavigation::AllAreas)
		.addFunction("GetAreaFromName", &ScriptingNavigation::GetAreaFromName)
		.addFunction("GetAreaCost", &ScriptingNavigation::GetAreaCost)
		.addFunction("SetAreaCost", &ScriptingNavigation::SetAreaCost)
		.addFunction("CalculatePath", &ScriptingNavigation::CalculatePath)
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

	if (script == nullptr)
	{
		ENGINE_CONSOLE_LOG("|[Error] The script sent to FillScriptInstanceComponentVars is NULLPTR, operation will be aborted");
		return;
	}
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
			ENGINE_CONSOLE_LOG("Tried to add a ''null_function'' to script component: %s of GameObject: %s", script->my_component->name.c_str(), aux_str.c_str());
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
			if (class_instances[i] == current_script)
				current_script = nullptr;

			delete class_instances[i];
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

			ENGINE_AND_SYSTEM_CONSOLE_LOG("Lua Resource for component %s (lua script component) is nullptr!",class_instances[i]->my_component->name.c_str());
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
			for (int i = 0; i < script_component->script_functions.size(); ++i)
			{
				if (script_component->script_functions[i].name == aux_str.c_str())
				{
					script->my_table_class[aux_str.c_str()](); // call to Lua to execute the given function
				}
			}
		}
	}
	else
	{
		ENGINE_CONSOLE_LOG("Can't callback %s since component has a null script instance", aux_str.c_str());
	}
}

void ModuleScripting::CompileDebugging()
{
	std::string abs_path = App->fs->GetBasePath();
	App->fs->NormalizePath(abs_path);

	std::size_t d_pos = 0;
	d_pos = abs_path.find("Debug");
	std::size_t r_pos = 0;
	r_pos = abs_path.find("Release");

	if (d_pos != 4294967295)  // If we are in DEBUG
	{
		abs_path = abs_path.substr(0, d_pos);
		abs_path += "Game/";
	}
	else if (r_pos != 4294967295) // If we are in RELEASE
	{
		abs_path = abs_path.substr(0, r_pos);
		abs_path += "Game/";
	}

	abs_path += "Lua_Debug";

	debug_path = abs_path;

	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scripting")
		.beginClass <ScriptingSystem>("System")
		.addConstructor<void(*) (void)>()
		.addFunction("GetDebuggingPath", &ScriptingSystem::GetDebuggingPath)
		.endClass()
		.endNamespace();

	std::string test = debug_path + "/IDE_Debugger.lua";
	bool compiled = luaL_dofile(L, test.c_str());

	if (compiled == LUA_OK) {
		//We don't need to do nothing here, LOG something at most
		ENGINE_CONSOLE_LOG("Compiled %s successfully!", test.c_str());
	}
	else {
		std::string error = lua_tostring(L, -1);
		ENGINE_CONSOLE_LOG("%s", error.data());
	}

	//debug_instance->my_table_class = luabridge::getGlobal(L, "GetTableDebug");
}

void ModuleScripting::StopDebugging()
{
	if (debug_instance != nullptr)
	{
			std::string path = debug_path + "/Debugger_Close.lua";
			luaL_dofile(L, path.c_str());
	}
}

void ModuleScripting::CallbackScriptFunctionParam(ComponentScript* script_component, const ScriptFunc& function_to_call, uint id)
{
	ScriptInstance* script = GetScriptInstanceFromComponent(script_component);

	std::string aux_str = function_to_call.name;
	if (script != nullptr)
	{
		if (App->GetAppState() == AppState::PLAY)
		{
			script->my_table_class[aux_str.c_str()](id); // call to Lua to execute the given function
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

	debug_instance = new ScriptInstance;

	return true;
}

bool ModuleScripting::Start() {
	return true;
}

bool ModuleScripting::CleanUp() {
	CleanUpInstances();

	if (debug_instance != nullptr)
	{
		delete debug_instance;
		debug_instance = nullptr;
	}

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
					current_script->awoken = true;
					current_script->my_table_class["Awake"]();	// Awake is done first, regardless of the script being active or not
				}
				else if (current_script->my_component->GetActive()) //Check if the script instance and it's object holding it are active
				{
					GameObject* GO = current_script->my_component->GetContainerGameObject();

					if (GO->GetActive()) // Even if the component script is active, if its gameobject its not, we won't update
					{
						if (current_script->started == false)
						{
							current_script->started = true;
							current_script->my_table_class["Start"]();	// Start is done only once for the first time the script is active
						}
						else
						{
							current_script->my_table_class["Update"]();	// Update is done on every iteration of the script as long as it remains active
							
							if(current_script != nullptr)
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

	return game_update;
}

void ModuleScripting::CleanUpInstances() {
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it) {
		if ((*it) != nullptr)
			delete (*it);
	}

	current_script = nullptr;
	//debug_instance->my_table_class = 0;

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

void ModuleScripting::LoadStatus(const json& file)
{
	if (App->isGame)
	{
		for (json::iterator it = file.begin(); it != file.end(); ++it)
		{
			// --- Retrieve GO's UID ---
			std::string Module = it.key().c_str();
			if (Module == "Scripting")
				continue;

			if (!file[it.key()]["LUA_Debug_Game"].is_null())
			{

			}

		}
	}
}

bool ModuleScripting::Stop() {
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		(*it)->awoken = (*it)->started = false;

	return true;
}
