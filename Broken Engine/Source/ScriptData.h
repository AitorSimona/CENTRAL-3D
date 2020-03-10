#ifndef __SCRIPTDATA_H__
#define __SCRIPTDATA_H__

#include <string>

#include "LuaIncludes.h"
#include "ComponentScript.h"

BE_BEGIN_NAMESPACE

//We don't want to include all the luabridge library in ModuleScripting.h so we don't include it everywhere where we include Module Scripting
struct BROKEN_API ScriptInstance {
	ComponentScript* my_component = nullptr;
	luabridge::LuaRef my_table_class = 0;
	bool awoken = false;
	bool started = false;
	bool script_is_null = false;
};

BE_END_NAMESPACE
#endif // __SCRIPTDATA_H__
