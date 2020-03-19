#include "ScriptingDebug.h"
#include "Application.h"
#include "ModuleTimeManager.h"

using namespace Broken;
ScriptingDebug::ScriptingDebug() {}
		 
ScriptingDebug::~ScriptingDebug() {}

//Function that Lua will be able to call as LOG
void ScriptingDebug::LogFromLua(const char* string) {
	ENGINE_CONSOLE_LOG("[Script]: %s", string);
}

float ScriptingDebug::GetRealDT() const {
	return App->time->GetRealTimeDt();
}

float ScriptingDebug::GetDT() const {
	return App->time->GetGameDt();
}

float ScriptingDebug::GameTime()
{
	return App->time->GetGameplayTimePassed();
}

// MATHS -------------------------------------------------------------
int ScriptingDebug::FloatNumsAreEqual(float a, float b)
{
	return (fabs(a - b) < std::numeric_limits<float>::epsilon());
}

int ScriptingDebug::DoubleNumsAreEqual(double a, double b)
{
	return (fabs(a - b) < std::numeric_limits<double>::epsilon());
}