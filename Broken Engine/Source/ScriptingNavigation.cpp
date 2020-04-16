#include "ScriptingNavigation.h"
#include "Application.h"
#include "ModuleDetour.h"
#include "ScriptData.h"
#include "MathGeoLib/include/Math/float3.h"

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

luabridge::LuaRef ScriptingNavigation::CalculatePath(float origin_x, float origin_y, float origin_z, float dest_x, float dest_y, float dest_z, int areaMask, lua_State* L) {
	std::vector<float3> path;
	float3 origin = { origin_x, origin_y, origin_z };
	float3 destination = { dest_x, dest_y, dest_z };

	luabridge::LuaRef ret(L, luabridge::newTable(L));
	int success = App->detour->calculatePath(origin, destination, areaMask, path);
	if (success > 0) {
		for (int i = 0; i < path.size(); ++i) {
			luabridge::LuaRef vector(L, luabridge::newTable(L));
			vector.append(path[i].x);
			vector.append(path[i].y);
			vector.append(path[i].z);
			ret.append(vector);
		}
	}

	return ret;
}
