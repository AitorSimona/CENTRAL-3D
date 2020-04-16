#ifndef __TRANSLATORUTILITIES_H__
#define __TRANSLATORUTILITIES_H__

#include <string>
#include <map>

//Fordward declare LuaRef
namespace luabridge
{
	class LuaRef;
}

struct CppTable 
{
	std::string type;

	std::map<std::string, double> vars_num;
	std::map<std::string, std::string> vars_string;
	std::map<std::string, bool> vars_bool;
};

//This header and respective cpp host the utilities to convert cpp info and structs into lua tables
//previously declared in the Globals.lua file. This allows us to for instance pass a Vec3 to lua, translating
//it in a way that in lua it can be accessed as var.x, var.y, var.z etc.
class CppLuaTranslatorUtilities
{
public:
	CppLuaTranslatorUtilities();
	~CppLuaTranslatorUtilities();

public: //Functions

	void PassCppTableValuesToLuaTable(CppTable cpp_table, luabridge::LuaRef lua_table);

};
#endif 
