#ifndef __SCRIPTINGTRANSFORM_H__
#define __SCRIPTINGTRANSFORM_H__

#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingTransform {
public:
	ScriptingTransform();
	~ScriptingTransform();

public:	
	// Position
	int GetPosition(lua_State* L, uint gameobject_UUID = -1);
	float GetPositionX(uint gameobject_UUID = -1) const;
	float GetPositionY(uint gameobject_UUID = -1) const;
	float GetPositionZ(uint gameobject_UUID = -1) const;

	void Translate(float x, float y, float z, bool local, uint gameobject_UUID = -1);
	void SetPosition(float x, float y, float z, bool local, uint gameobject_UUID = -1);

	// Rotation
	void RotateObject(float x, float y, float z, uint gameobject_UUID = -1);
	void SetObjectRotation(float x, float y, float z, uint gameobject_UUID = -1);
	void LookAt(float spotX, float spotY, float spotZ, bool local, uint gameobject_UUID = -1);

	int GetRotation(lua_State* L, uint gameobject_UUID = -1) const;
	float GetRotationX(uint gameobject_UUID = -1) const;
	float GetRotationY(uint gameobject_UUID = -1) const;
	float GetRotationZ(uint gameobject_UUID = -1) const;
};
BE_END_NAMESPACE
#endif // __SCRIPTINGTRANSFORM_H__