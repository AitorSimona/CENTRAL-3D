#include "EngineLog.h"

BE_BEGIN_NAMESPACE
void ExLogCompilationFirstMessage() {
	BrokenEngine::LogCompilationFirstMessage();
}

void ExEngineConsoleLog(const char file[], int line, const char* format, ...) {
	va_list args;
	va_start(args, format);
	BrokenEngine::EngineConsoleLog(file, line, format, args);
	va_end(args);
}

void ExSystemConsoleLog(const char file[], int line, const char* format, ...) {
	va_list args;
	va_start(args, format);
	BrokenEngine::SystemConsoleLog(file, line, format, args);
	va_end(args);
}
BE_END_NAMESPACE