#ifndef __SCRIPTINGSYSTEM_H__
#define __SCRIPTINGSYSTEM_H__

#include "BrokenCore.h"

class lua_State;

namespace luabridge {
	class LuaRef;
}

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingSystem {
public:
	ScriptingSystem();
	~ScriptingSystem();

public:

	void LogFromLua(const char* string);
	float GetRealDT() const;
	float GetDT() const;
	float GameTime();
	void PauseGame();
	void ResumeGame();
	const char* GetDebuggingPath();

	// Maths
	int FloatNumsAreEqual(float a, float b);
	int DoubleNumsAreEqual(double a, double b);
	double RandomNumber();
	double RandomNumberInRange(double min, double max);
	luabridge::LuaRef RandomNumberList(int listSize, double min, double max, lua_State* L);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGSYSTEM_H__