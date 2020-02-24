#ifndef __SCRIPTDATA_H__
#define __SCRIPTDATA_H__

#include <string>

#include "LuaIncludes.h"
#include "ComponentScript.h"


//We don't want to include all the luabridge library in ModuleScripting.h so we don't include it everywhere where we include Module Scripting
struct ScriptFile {
	std::string name;
	std::string full_path;
	uint32 resource_uid;
};

struct ScriptInstance {
	ComponentScript* my_component = nullptr;
	ResourceScript* my_resource = nullptr;
	luabridge::LuaRef my_table_class = 0;
	bool awoken = false;
	bool started = false;
};

#endif // __SCRIPTDATA_H__

