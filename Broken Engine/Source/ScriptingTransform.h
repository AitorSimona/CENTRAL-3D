#ifndef __SCRIPTINGTRANSFORM_H__
#define __SCRIPTINGTRANSFORM_H__

#include "BrokenCore.h"

class lua_State;

namespace luabridge
{
	class LuaRef;
}

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingTransform {
public:
	ScriptingTransform();
	~ScriptingTransform();

public:	
	// Position
	luabridge::LuaRef GetPosition(lua_State* L);
	void Translate(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	// Rotation
	luabridge::LuaRef GetRotation(lua_State* L);
	void RotateObject(float x, float y, float z);
	void SetObjectRotation(float x, float y, float z);
	void LookAt(float spotX, float spotY, float spotZ);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGTRANSFORM_H__