#ifndef __SCRIPTINGSYSTEM_H__
#define __SCRIPTINGSYSTEM_H__

#include "BrokenCore.h"

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
};
BE_END_NAMESPACE
#endif // __SCRIPTINGSYSTEM_H__