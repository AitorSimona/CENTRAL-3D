#ifndef __SCRIPTINGGAMEOBJECT_H__
#define __SCRIPTINGGAMEOBJECT_H__

#include "BrokenCore.h"
class lua_State;

namespace luabridge
{
	class LuaRef;
}

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingGameobject {
public:
	ScriptingGameobject();
	~ScriptingGameobject();

public:

	// GameObject-related functions
	uint FindGameObject(const char* go_name);
	void DestroyGOFromScript(uint gameobject_UUID);

	float GetGameObjectPos(uint gameobject_UUID, lua_State* L);
	float GetGameObjectPosX(uint gameobject_UUID);
	float GetGameObjectPosY(uint gameobject_UUID);
	float GetGameObjectPosZ(uint gameobject_UUID);
	void TranslateGameObject(uint gameobject_UUID, float x, float y, float z);

	uint GetComponentFromGO(const char* component_type, const char* go_name = "NO_NAME");
	
	/*template <typename T, typename U>
	void ConvertVectorToTable(lua_State* L, T begin, U end) {
		lua_newtable(L);
		for (size_t i = 0; begin != end; ++begin, ++i) {
			lua_pushinteger(L, i + 1);
			lua_pushnumber(L, *begin);
			lua_settable(L, -3);
		}
	}

	template <typename T, typename U>
	void ConvertTableToVector(lua_State* L, T begin, U end) {
		
		assert(lua_istable(L, -1));
		for (size_t i = 0; begin != end; ++begin, ++i) {
			lua_pushinteger(L, i + 1);
			lua_gettable(L, -2);
			*begin = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	}*/

	// Current Camera
	int GetPosInFrustum(float x, float y, float z, float fovratio1, float fovratio2);
	int GetFrustumPlanesIntersection(float x, float y, float z, lua_State* luaSt);

	// Get a script file
	luabridge::LuaRef GetScript(uint gameobject_UUID, lua_State* L);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGGAMEOBJECT_H__