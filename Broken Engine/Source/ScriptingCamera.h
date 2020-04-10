#ifndef __SCRIPTINGCAMERA_H__
#define __SCRIPTINGCAMERA_H__

#include "BrokenCore.h"
class lua_State;

namespace luabridge
{
	class LuaRef;
}

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingCamera {
public:
	ScriptingCamera();
	~ScriptingCamera();

public:

	// Current Camera
	int GetPosInFrustum(float x, float y, float z, float fovratio1, float fovratio2);
	luabridge::LuaRef GetFrustumPlanesIntersection(float x, float y, float z, float fovratio, lua_State* L);

	// Screen <--> World coordinates
	luabridge::LuaRef WorldToScreen(float x, float y, float z, lua_State* L);
	luabridge::LuaRef ScreenToWorld(float x, float y, float distance, lua_State* L);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGCAMERA_H__