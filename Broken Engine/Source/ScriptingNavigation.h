#ifndef __SCRIPTINGNAVIGATION_H__
#define __SCRIPTINGNAVIGATION_H__
#pragma once

#include "BrokenCore.h"
#include "MathGeoLib/include/Math/float3.h"

class lua_State;

BE_BEGIN_NAMESPACE

class BROKEN_API ScriptingNavigation {
public:
	ScriptingNavigation();
	~ScriptingNavigation();

	int AllAreas();
	int GetAreaFromName(const char* name);
	int GetAreaCost(int areaIndex);
	void SetAreaCost(int areaIndex, float areaCost);

	int CalculatePath(float3 origin, float3 destination, int areaMask, lua_State* L);
};

BE_END_NAMESPACE

#endif

