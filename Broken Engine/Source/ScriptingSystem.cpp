#include "ScriptingSystem.h"
#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleScripting.h"
#include "ModuleGui.h"
#include "RandomGenerator.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingSystem::ScriptingSystem() {}
		 
ScriptingSystem::~ScriptingSystem() {}

//Function that Lua will be able to call as LOG
void ScriptingSystem::LogFromLua(const char* string) {
	ENGINE_CONSOLE_LOG("[Script]: %s", string);
}

float ScriptingSystem::GetRealDT() const {
	return App->time->GetRealTimeDt();
}

float ScriptingSystem::GetDT() const {
	return App->time->GetGameDt();
}

float ScriptingSystem::GameTime()
{
	return App->time->GetGameplayTimePassed();
}

void ScriptingSystem::PauseGame()
{
	App->time->gamePaused = true;
}

void ScriptingSystem::ResumeGame()
{
	App->time->gamePaused = false;
}

const char* ScriptingSystem::GetDebuggingPath()
{
	return App->scripting->debug_path.c_str();
}

// MATHS -------------------------------------------------------------
int ScriptingSystem::FloatNumsAreEqual(float a, float b)
{
	return (fabs(a - b) < std::numeric_limits<float>::epsilon());
}

int ScriptingSystem::DoubleNumsAreEqual(double a, double b)
{
	return (fabs(a - b) < std::numeric_limits<double>::epsilon());
}

double ScriptingSystem::RandomNumber()
{
	return App->RandomNumberGenerator.GetDoubleRN();
}

double ScriptingSystem::RandomNumberInRange(double min, double max)
{
	return App->RandomNumberGenerator.GetDoubleRNinRange(min, max);
}

luabridge::LuaRef ScriptingSystem::RandomNumberList(int listSize, double min, double max, lua_State* L)
{
	std::vector<double> numbers = App->RandomNumberGenerator.GetDoubleRNVecinRange(listSize, min, max);

	luabridge::LuaRef ret = 0;
	luabridge::LuaRef uniforms(L, luabridge::newTable(L));
	for (int i = 0; i < numbers.size(); ++i)
	{
		uniforms.append(numbers[i]);
	}

	ret = uniforms;

	return ret;
}