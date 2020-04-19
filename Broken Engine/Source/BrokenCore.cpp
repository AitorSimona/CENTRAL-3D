#include "BrokenCore.h"
#include "Application.h"

#include <iostream>
#define NOMINMAX
#include <Windows.h>

BE_BEGIN_NAMESPACE
// INFORMATION LOG ----------------------------------------------------------------------------
void EngineConsoleLog(const char file[], int line, const char* format, ...) {
	static char tmp_string[MAX_BUF_SIZE];
	static char tmp_string2[MAX_BUF_SIZE];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, MAX_BUF_SIZE, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, MAX_BUF_SIZE, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (App) {
		sprintf_s(tmp_string2, MAX_BUF_SIZE, "\n%s", tmp_string);
		App->Log(tmp_string2);
	}
}

void SystemConsoleLog(const char file[], int line, const char* format, ...) {
	static char tmp_string[MAX_BUF_SIZE];
	static char tmp_string2[MAX_BUF_SIZE];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, MAX_BUF_SIZE, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, MAX_BUF_SIZE, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	std::cout << tmp_string2 << std::endl;
}

void LogCompilationFirstMessage() {
	std::cout << std::endl << std::endl << std::endl << std::endl << "-- Compilation Start Succeeded --" << std::endl <<
		"	Compilation at Date " << __DATE__ <<
		" and Time " << __TIME__ <<
		std::endl << std::endl << std::endl << std::endl;

	if (App) {
		std::string data = "\n\n\n\n---------------------------------------------------------------------------------------------\n\n";
		data += "-- Compilation Start Succeeded --\n		Compilation at Date ";

		data += __DATE__;
		data += " and Time ";
		data += __TIME__;
		data += "\n\n---------------------------------------------------------------------------------------------\n\n\n\n";

		App->Log(data.c_str());
	}
}
BE_END_NAMESPACE