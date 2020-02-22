#ifndef __SCRIPTVAR_H__
#define __SCRIPTVAR_H__

#include <string>

enum class VarType
{
	NONE = 0,
	BOOLEAN,
	STRING,
	DOUBLE			//Remember that Lua doesn't have integers & floats, it uses long floats to represent all numbers
};

union VarValue
{
	bool as_boolean;
	char as_string[256] = "null";
	double as_double_number;

	//VarValue(const char str) { as_string = str; }
};

//This struct holds the data of a variable inside our associated LUA script but in C++
class ScriptVar 
{
public:
	ScriptVar();
	ScriptVar(bool value);
	ScriptVar(const char* value);
	ScriptVar(double value);
	~ScriptVar();

public:
	void ChangeEditorValue(bool value);
	void ChangeEditorValue(const char* value);
	void ChangeEditorValue(double number);
public:
	//Type of the variable
	VarType type = VarType::NONE;

	VarValue editor_value;
	VarValue script_defined_value;

	std::string name;
	//Bool to see if the value was changed in the editor
	bool changed_value = false;
};

#endif // __SCRIPTVAR_H__
