#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "Source/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	void SetWindowWidth(uint width);
	void SetWindowHeight(uint height);
	uint GetWindowWidth();
	uint GetWindowHeight();
	uint GetDisplayRefreshRate();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:

	// --- Display ---
	uint				RefreshRate = 0;
	uint				screen_width = 1280;
	uint				screen_height = 1024;
};

#endif // __ModuleWindow_H__