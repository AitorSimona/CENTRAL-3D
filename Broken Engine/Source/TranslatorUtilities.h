#ifndef __TRANSLATORUTILITIES_H__
#define __TRANSLATORUTILITIES_H__

#include <string>
#include <map>

//Fordward declare LuaRef
namespace luabridge
{
	class LuaRef;
}
namespace math
{
	class float3;
}

//This struct can pass all of its vars to a lua_table calling PassCppTableValuesToLuaTable, so we can fill it as any table in Globals.lua
struct CppTable 
{
	std::string type;

	//We store duples like lua does, the first parameter is the key of the var in lua for instance for Blackboard.Health with a value of 100 we would
	//emplace a member into the map vars_num ("Health",100) and the same goes for strings and bools
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
	CppTable GetCppTableFromVec3(math::float3 vec3);

};
#endif 
