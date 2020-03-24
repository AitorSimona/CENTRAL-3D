#ifndef __SCRIPTINGANIMATIONS_H__
#define __SCRIPTINGANIMATIONS_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingAnimations {
public:
	ScriptingAnimations();
	~ScriptingAnimations();

public:
	void StartAnimation(const char* name, float speed);
	void SetAnimSpeed(const char* name, float speed);
	void SetCurrentAnimSpeed(float speed);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGANIMATIONS_H__