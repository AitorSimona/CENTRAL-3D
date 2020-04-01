#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"
#include <vector>

class lua_State;
BE_BEGIN_NAMESPACE

class GameObject;
class ComponentScript;
struct ScriptInstance;
struct ScriptFunc;

enum _AppState;
class BROKEN_API ModuleScripting : public Module {
	friend class ScriptingElements;
public:
	ModuleScripting(bool start_enabled = true);
	~ModuleScripting();

public:
	//template <typename T, typename U>
	//void ConvertVectorToTable(lua_State* L, T begin, U end);

	//template <typename T, typename U>
	//void ConvertTableToVector(lua_State* L, T begin, U end);

	bool DoHotReloading();
	bool JustCompile(std::string absolute_path);
	void CompileScriptTableClass(ScriptInstance* script);
	void SendScriptToModule(ComponentScript* script_component);
	void FillScriptInstanceComponentVars(ScriptInstance* script);
	void FillScriptInstanceComponentFuncs(ScriptInstance* script);
	void DeleteScriptInstanceWithParentComponent(ComponentScript* script_component);
	void NullifyScriptInstanceWithParentComponent(ComponentScript* script_component);
	void NotifyHotReloading();
	bool CheckEverythingCompiles();
	void CallbackScriptFunction(ComponentScript* script_component, const ScriptFunc& function_to_call);
	void CompileDebugging();
	void StopDebugging();
	void CallbackScriptFunctionParam(ComponentScript* script_component, const ScriptFunc& function_to_call, uint id);

	void CleanUpInstances();

	bool Init(json& file) override;
	bool Start();
	bool CleanUp();

	update_status Update(float dt) override;
	update_status GameUpdate(float gameDT);

	bool Stop() override;
	ScriptInstance* GetScriptInstanceFromComponent(ComponentScript* component_script);

public:
	ScriptInstance* current_script;
	update_status game_update = UPDATE_CONTINUE;
	std::string debug_path = "null";

private:
	// L is our Lua Virtual Machine, it's called L because its the common name it receives, so all programers can understand what this var is
	lua_State* L = nullptr;
	bool start = true;
	bool cannot_start = false; //We cannot start if a compilation error would cause a crash on the engine when we start playing
	bool hot_reloading_waiting = false;

	_AppState previous_AppState = (_AppState)2; // we use the EDITOR value of the script (can't include application.h because it would slow down compilation time)

	ScriptInstance* debug_instance = nullptr;

	std::vector<ScriptInstance*> recompiled_instances;
	std::vector<ScriptInstance*> class_instances;
};
BE_END_NAMESPACE
#endif // !__MODULESCRIPTING_H__
