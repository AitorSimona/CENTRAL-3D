#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"
#include <vector>

class lua_State;
class GameObject;
class ComponentScript;
struct ScriptFile;
struct ScriptInstance;

enum _AppState;

class ModuleScripting : public Module
{
public:
	ModuleScripting(bool start_enabled = true);
	~ModuleScripting();

public:
	bool DoHotReloading();
	bool JustCompile(std::string absolute_path);
	void CompileScriptTableClass(ScriptInstance* script);
	void SendScriptToModule(ComponentScript* script_component);
	ScriptFile* AddScriptFile(ComponentScript* script_component, std::string full_file_path);
	void FillScriptInstanceComponentVars(ScriptInstance* script);
	void DeleteScriptInstanceWithParentComponent(ComponentScript* script_component);
	void NullifyScriptInstanceWithParentComponent(ComponentScript* script_component);
	void NotifyHotReloading();
	bool CheckEverythingCompiles();


public:
	bool Init(json file) override;
	bool Start();
	bool CleanUp();

	update_status Update(float dt) override;
	update_status GameUpdate(float gameDT);

	void Stop();

public:
	ScriptInstance* current_script;

private:
	// L is our Lua Virtual Machine, it's called L because its the common name it receives, so all programers can understand what this var is
	lua_State *L = nullptr;
	bool start = true;
	bool cannot_start = false; //We cannot start if a compilation error would cause a crash on the engine when we start playing
	bool hot_reloading_waiting = false;

	_AppState previous_AppState = (_AppState)2; // we use the EDITOR value of the script (can't include application.h because it would slow down compilation time)

	void CleanUpInstances();

	std::vector<ScriptFile*> script_files;
	std::vector<ScriptInstance*> recompiled_instances;
	std::vector<ScriptInstance*> class_instances;
};


#endif // !__MODULESCRIPTING_H__
