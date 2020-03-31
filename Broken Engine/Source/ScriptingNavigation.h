#ifndef __SCRIPTINGNAVIGATION_H__
#define __SCRIPTINGNAVIGATION_H__
#pragma once

#include "BrokenCore.h"

class lua_State;
namespace luabridge
{
	class LuaRef;
}


BE_BEGIN_NAMESPACE

class BROKEN_API ScriptingNavigation {
public:
	ScriptingNavigation();
	~ScriptingNavigation();

	int AllAreas();
	int GetAreaFromName(const char* name);
	int GetAreaCost(int areaIndex);
	void SetAreaCost(int areaIndex, float areaCost);

	luabridge::LuaRef CalculatePath(float origin_x, float origin_y, float origin_z, float dest_x, float dest_y, float dest_z , int areaMask, lua_State* L);
};

BE_END_NAMESPACE

#endif

