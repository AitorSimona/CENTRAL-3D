#ifndef __SCRIPTVAR_H__
#define __SCRIPTVAR_H__

#include <string>
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
enum class BROKEN_API VarType {
	NONE = 0,
	BOOLEAN,
	STRING,
	DOUBLE			//Remember that Lua doesn't have integers & floats, it uses long floats to represent all numbers
};

union BROKEN_API VarValue {
	bool as_boolean;
	char as_string[256] = "null";
	double as_double;
};
//This struct holds the data of a variable inside our associated LUA script but in C++
class BROKEN_API ScriptVar {
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

	bool operator == (const ScriptVar &comp) const;

public:
	// Type of the variable
	VarType type = VarType::NONE;

	VarValue editor_value;
	VarValue script_defined_value;

	std::string name = "";
	std::string object_name = "";
	bool display_object_name = false;

	// Bool to see if the value was changed in the editor
	bool changed_value = false;
};

//This data structure is created in order to restrict possibilities from callbacks and avoid users/programmers from crashing the engine
struct BROKEN_API ScriptFunc
{
	std::string name = "null_function";

};
BE_END_NAMESPACE
#endif // __SCRIPTVAR_H__
