#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

#define CONSOLE_LOG(format, ...) _log(__FILE__, __LINE__, format, __VA_ARGS__);

void _log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};


// Configuration -----------
#define TITLE "CENTRAL 3D"
#define VERSION "0.05"
#define ASSETS_FOLDER "Assets/"
#define SETTINGS_FOLDER "Settings/"
#define LIBRARY_FOLDER "Library/"
#define TEXTURES_FOLDER "Library/Textures/"
#define MESHES_FOLDER "Library/Meshes/"
#define SCENES_FOLDER "Assets/Scenes/"
#define MODELS_FOLDER "Library/Models/"

#define SCREEN_SIZE 1
#define MAX_BUF_SIZE 4096