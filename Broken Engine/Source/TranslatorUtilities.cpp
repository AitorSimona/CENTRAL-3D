#include "TranslatorUtilities.h"
#include "LuaIncludes.h"

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
		if (lua_table["type"] != cpp_table.type.c_str())
			ENGINE_CONSOLE_LOG("Attempted to pass a cpp_table of type %s into a lua table of type ",cpp_table.type, lua_table["type"]);
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
