#ifndef __SCRIPTINGANIMATIONS_H__
#define __SCRIPTINGANIMATIONS_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingAnimations {
public:
	ScriptingAnimations();
	~ScriptingAnimations();

public:
	void StartAnimation(const char* name, float speed, uint gameobject_UUID);
	void SetAnimSpeed(const char* name, float speed, uint gameobject_UUID);
	void SetCurrentAnimSpeed(float speed,uint gameobject_UUID);
	void SetBlendTime(float value, uint gameobject_UUID);
	int CurrentAnimEnded(uint gameobject_UUID);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGANIMATIONS_H__
