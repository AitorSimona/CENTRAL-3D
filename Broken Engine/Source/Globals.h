#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

//Just in case -- Null redefinition
#ifdef NULL
#undef NULL
#endif
#define NULL 0
#define NULLRECT {0,0,0,0}


// LOGGING -----------------------------------------------------------------------
//Visual Studio Console will log both system & engine consoles!!!!!!!!!!!
/// Print only in Engine Console
#define ENGINE_CONSOLE_LOG(format, ...) EngineConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// Print only in System Console
#define SYSTEM_CONSOLE_LOG(format, ...) SystemConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// Print in both Consoles
#define ENGINE_AND_SYSTEM_CONSOLE_LOG(format, ...) EngineConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__); SystemConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// First info at Compilation
#define COMPILATIONLOGINFO LogCompilationFirstMessage()

/// Log functions
void LogCompilationFirstMessage();
void EngineConsoleLog(const char file[], int line, const char* format, ...);
void SystemConsoleLog(const char file[], int line, const char* format, ...);
// -------------------------------------------------------------------------------


// ERROR HANDLING ----------------------------------------------------------------


// -------------------------------------------------------------------------------


// PREDEFINED & GENERAL VARIABLES ------------------------------------------------
/// Keep a value between 0.0f and 1.0f
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI			3.14159265359f
#define TO_BOOL( a )  ( (a != 0) ? true : false )

/// Useful helpers, InRange checker, min, max and clamp
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))
#define IN_RANGE( value, min, max ) ( ((value) >= (MIN) && (value) <= (MAX)) ? 1 : 0 )

/// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

/// Camera Settings
#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

/// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

/// Releasing Memory
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }
// -------------------------------------------------------------------------------


// TYPEDEFS ----------------------------------------------------------------------
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
// -------------------------------------------------------------------------------


// OTHERS ------------------------------------------------------------------------
enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};


/// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

/// Swaps 2 values
template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}
// -------------------------------------------------------------------------------


// CONFIGURATION -----------------------------------------------------------------
#define TITLE "CENTRAL 3D"
#define VERSION "v2.0"
#define ASSETS_FOLDER "Assets/"
#define SETTINGS_FOLDER "Settings/"
#define LIBRARY_FOLDER "Library/"
#define TEXTURES_FOLDER "Library/Textures/"
#define MESHES_FOLDER "Library/Meshes/"
#define SCENES_FOLDER "Assets/Scenes/"
#define MODELS_FOLDER "Library/Models/"
#define SHADERS_FOLDER "Library/Shaders/"

#define SCREEN_SIZE 1
#define MAX_BUF_SIZE 4096
// -------------------------------------------------------------------------------