#ifndef __SCRIPTINGELEMENTS_H__
#define __SCRIPTINGELEMENTS_H__
#include "ModuleInput.h"

#include "BrokenCore.h"

class lua_State;

namespace luabridge
{
	class LuaRef;
}

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingElements {
public:
	ScriptingElements();
	~ScriptingElements();

public:

	// GameObject-related functions
	uint FindGameObject(const char* go_name);
	float GetGameObjectPos(uint gameobject_UID, lua_State* L);
	float GetGameObjectPosX(uint gameobject_UID);
	float GetGameObjectPosY(uint gameobject_UID);
	float GetGameObjectPosZ(uint gameobject_UID);
	void TranslateGameObject(uint gameobject_UID, float x, float y, float z);

	// Position
	float GetPositionX() const;
	float GetPositionY() const;
	float GetPositionZ() const;
	int GetPosition(lua_State* L);

	void Translate(float x, float y, float z, bool local);
	void SetPosition(float x, float y, float z, bool local);

	// Rotation
	void RotateObject(float x, float y, float z);
	void SetObjectRotation(float x, float y, float z);
	void LookAt(float spotX, float spotY, float spotZ, bool local);

	int GetRotation(bool local, lua_State* L) const;
	float GetRotationX() const;
	float GetRotationY() const;
	float GetRotationZ() const;

	// Current Camera
	int GetPosInFrustum(float x, float y, float z);

	//Get a script file
	luabridge::LuaRef GetScript(uint go_UID, lua_State* L);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGELEMENTS_H__