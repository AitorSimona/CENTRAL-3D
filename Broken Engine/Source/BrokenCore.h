#ifndef __BE_BROKENCORE_H__
#define __BE_BROKENCORE_H__

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include "glad/include/glad/glad.h"
//#include <windows.h>
#include "Errors.h"
#include <stdio.h>

//Import/export define
#ifdef BE_BUILD_DLL
	#define BROKEN_API __declspec(dllexport)
#else
	#define BROKEN_API __declspec(dllimport)
#endif

#define BE_BEGIN_NAMESPACE namespace Broken {
#define BE_END_NAMESPACE }

//Just in case -- Null redefinition
#ifdef NULL
#undef NULL
#endif
#define NULL 0
#define NULLRECT {0,0,0,0}

// LOGGING -----------------------------------------------------------------------
// Visual Studio Console will log both system & engine consoles!!!!!!!!!!!
/// Print only in Engine Console
#define ENGINE_CONSOLE_LOG(format, ...) Broken::EngineConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// Print only in System Console
#define SYSTEM_CONSOLE_LOG(format, ...) Broken::SystemConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// Print in both Consoles
#define ENGINE_AND_SYSTEM_CONSOLE_LOG(format, ...) Broken::EngineConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__); Broken::SystemConsoleLog(__FILE__, __LINE__, format, __VA_ARGS__)
/// First info at Compilation
#define COMPILATIONLOGINFO Broken::LogCompilationFirstMessage()

BE_BEGIN_NAMESPACE
/// Log functions

void LogCompilationFirstMessage();
void EngineConsoleLog(const char file[], int line, const char* format, ...);
void SystemConsoleLog(const char file[], int line, const char* format, ...);
BE_END_NAMESPACE
// -------------------------------------------------------------------------------


// ERROR HANDLING ----------------------------------------------------------------
/// Error Handling Macros
#ifdef _DEBUG
#define GLCall(x) GLClearError(); x; CRONOS_ASSERT(GLLogCall(#x, __FILE__, __LINE__));
#define GL_SETERRORHANDLER(majV, minV) SetErrorHandler(majV, minV)
#define BROKEN_ASSERT(x, ...) if(!x) { SYSTEM_CONSOLE_LOG("BROKEN ENGINE ASSERT: ", __VA_ARGS__); __debugbreak(); }
#define BROKEN_WARN(x, ...) if(!x) { SYSTEM_CONSOLE_LOG("BROKEN ENGINE WARN: ",  __VA_ARGS__); }
#else
#define GLCall(x) x;
#define GL_SETERRORHANDLER(majV, minV) SYSTEM_CONSOLE_LOG("\n\n\nGL_SETERROHANDLER (glDebugMessageCallback) not Available IN DEBUG CONTEXT\n\n\n");
#define BROKEN_ASSERT(x, ...) if(!x) { ENGINE_AND_SYSTEM_CONSOLE_LOG("BROKEN ENGINE ASSERT: ", __VA_ARGS__); __debugbreak(); }
#define BROKEN_WARN(x, ...) if(!x) { ENGINE_AND_SYSTEM_CONSOLE_LOG("BROKEN ENGINE WARN: ",  __VA_ARGS__) }
#endif
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
#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

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
typedef unsigned __int16 uint16;
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
#define SHADERS_ASSETS_FOLDER "Assets/Shaders/"
#define SCRIPTS_FOLDER "Library/Scripts/"
#define SOUNDS_FOLDER "Assets/Sounds/"
#define ANIMATIONS_FOLDER "Library/Animations/"
#define ANIMATOR_FOLDER "Library/Animator/"
#define BONES_FOLDER "Library/Bones/"
#define FONTS_FOLDER "Assets/Fonts/"
#define NAVMESH_FOLDER "Assets/NavMeshes/"
#define BUILDS_FOLDER "Builds/"
#define GAME_EXE "BEG.exe"
#define LUA_GLOBALS "Lua_Globals/"

#define SCREEN_SIZE 1
#define MAX_BUF_SIZE 4096
// -------------------------------------------------------------------------------
#endif