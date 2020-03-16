#include "ScriptVar.h"
#include "BrokenCore.h"


using namespace Broken;
ScriptVar::ScriptVar() {

}

ScriptVar::ScriptVar(bool value) {
	type = VarType::BOOLEAN;

	script_defined_value.as_boolean = value;
	editor_value.as_boolean = value;
}

ScriptVar::ScriptVar(const char* value) {
	type = VarType::STRING;

	strcpy_s(script_defined_value.as_string, value);
	strcpy_s(editor_value.as_string, value);
}

ScriptVar::ScriptVar(double value) {
	type = VarType::DOUBLE;

	script_defined_value.as_double = value;
	editor_value.as_double = value;
}

ScriptVar::~ScriptVar() {

}

void ScriptVar::ChangeEditorValue(bool value) {
	if (type == VarType::BOOLEAN) {
		editor_value.as_boolean = value;
		changed_value = true;
	}
	else
		ENGINE_CONSOLE_LOG("Tried to give a boolean value to a var with different type!");
}

void ScriptVar::ChangeEditorValue(const char* value) {
	if (type == VarType::STRING) {
		strcpy(editor_value.as_string, value);
		changed_value = true;
	}
	else
		ENGINE_CONSOLE_LOG("Tried to give a string value to a var with different type!");
}

void ScriptVar::ChangeEditorValue(double value)
{
	if (type == VarType::DOUBLE)
	{
		editor_value.as_double = value;
		changed_value = true;
	}
	else
		ENGINE_CONSOLE_LOG("Tried to give a double float value to a var with different type!");
}

bool ScriptVar::operator == (const ScriptVar &comp) const {
	return (comp.type == type && comp.name == name);
}