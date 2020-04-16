#include "TranslatorUtilities.h"
#include "LuaIncludes.h"
#include	"MathGeoLib/include/Math/float3.h"

CppLuaTranslatorUtilities::CppLuaTranslatorUtilities()
{
}

CppLuaTranslatorUtilities::~CppLuaTranslatorUtilities()
{
}

//Pass the values of a CppTable into a given Table
void CppLuaTranslatorUtilities::PassCppTableValuesToLuaTable(CppTable cpp_table, luabridge::LuaRef lua_table)
{
	if (lua_table["type"].isString())
	{
		std::string aux_str = lua_table["type"];
		if (aux_str.compare(cpp_table.type))
		{
			ENGINE_CONSOLE_LOG("!Attempted to pass a cpp_table of type %s into a lua table of type %s", cpp_table.type.c_str(), aux_str.c_str());
		}
	}

	//Iterate the variables in the cpp table & pass them to the lua table corresponding variable
	//Iterate numbers (double)
	std::map<std::string, double>::iterator it_num = cpp_table.vars_num.begin();
	for (; it_num != cpp_table.vars_num.end(); ++it_num)
	{
		lua_table[(*it_num).first.c_str()] = (*it_num).second;
	}

	//Iterate the strings
	std::map<std::string, std::string>::iterator it_str = cpp_table.vars_string.begin();
	for (; it_str != cpp_table.vars_string.end(); ++it_str)
	{
		lua_table[(*it_str).first.c_str()] = (*it_str).second.c_str();
	}

	//Iterate the booleans
	std::map<std::string, bool>::iterator it_bool = cpp_table.vars_bool.begin();
	for (; it_bool != cpp_table.vars_bool.end(); ++it_bool)
	{
		lua_table[(*it_bool).first.c_str()] = (*it_bool).second;
	}
}

CppTable CppLuaTranslatorUtilities::GetCppTableFromVec3(float3 vec3)
{
	CppTable ret;

	//Since Lua uses the table for everythig it is nice to know what we are dealing with, if the designer didn't mention or place
	//a type in the table he has prepared in the Globals.lua PLACE/CHOOSE ONE YOURSELF. The type here and at Globals.lua MUST be identical, you will be warned in the engine console if you make a typo
	ret.type = "Vec3";

	ret.vars_num.emplace("x",vec3.x);
	ret.vars_num.emplace("y", vec3.y);
	ret.vars_num.emplace("z", vec3.z);

	return ret;
}
