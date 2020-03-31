#include "ScriptingNavigation.h"
#include "Application.h"
#include "ModuleDetour.h"
#include "ScriptData.h"

using namespace Broken;

ScriptingNavigation::ScriptingNavigation() {}

ScriptingNavigation::~ScriptingNavigation() {}

int ScriptingNavigation::AllAreas() {
	return App->detour->allAreas();
}

int ScriptingNavigation::GetAreaFromName(const char* name) {
	return App->detour->getAreaFromName(name);
}

int ScriptingNavigation::GetAreaCost(int areaIndex) {
	return App->detour->getAreaCost(areaIndex);
}

void ScriptingNavigation::SetAreaCost(int areaIndex, float areaCost) {
	App->detour->setAreaCost(areaIndex, areaCost);
}

int ScriptingNavigation::CalculatePath(float3 origin, float3 destination, int areaMask, lua_State* L) {
	std::vector<float3> path;
	int ret = App->detour->calculatePath(origin, destination, areaMask, path);
	if (ret > 0) {
		for (int i = 0; i = path.size(); ++i) {
			lua_pushnumber(L, path[i].x);
			lua_pushnumber(L, path[i].y);
			lua_pushnumber(L, path[i].z);
		}
	}
	else
		ret = 0;

	return ret;
}
