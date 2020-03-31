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
	int GetPosition(lua_State* L);
	float GetPositionX() const;
	float GetPositionY() const;
	float GetPositionZ() const;

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
};
BE_END_NAMESPACE
#endif // __SCRIPTINGTRANSFORM_H__