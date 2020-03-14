#ifndef __SCRIPTINGDEBUG_H__
#define __SCRIPTINGDEBUG_H__
#include "ModuleInput.h"

#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingDebug {
public:
	ScriptingDebug();
	~ScriptingDebug();

public:

	void LogFromLua(const char* string);
	float GetRealDT() const;
	float GetDT() const;
	float GameTime();

	// Maths
	int FloatNumsAreEqual(float a, float b);
	int DoubleNumsAreEqual(double a, double b);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGDEBUG_H__