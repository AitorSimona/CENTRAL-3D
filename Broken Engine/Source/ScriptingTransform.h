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
	luabridge::LuaRef GetPosition(uint gameobject_UUID, lua_State* L);
	void Translate(float x, float y, float z, uint gameobject_UUID);
	void SetPosition(float x, float y, float z, uint gameobject_UUID);

	// Rotation
	luabridge::LuaRef GetRotation(uint gameobject_UUID, lua_State* L);
	void RotateObject(float x, float y, float z, uint gameobject_UUID);
	void SetObjectRotation(float x, float y, float z, uint gameobject_UUID);
	void LookAt(float spotX, float spotY, float spotZ, uint gameobject_UUID);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGTRANSFORM_H__