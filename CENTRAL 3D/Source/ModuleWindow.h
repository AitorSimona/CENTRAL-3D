#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(json file);
	bool Start() override;
	bool CleanUp();

	// On Resize
	void UpdateWindowSize() const;

	// --- Setters ---
	void SetFullscreen(bool value);
	void SetResizable(bool value);
	void SetBorderless(bool value);
	void SetFullscreenDesktop(bool value);
	void SetWinBrightness(float value);
	void SetTitle(const char* title);
	void SetWindowWidth(uint width);
	void SetWindowHeight(uint height);

	// --- Only App should access this, through SetAppName ---
	void SetWinTitle(const char* name);

	// --- Getters ---
	void GetWinMaxMinSize(uint & min_width, uint & min_height, uint & max_width, uint & max_height) const;
	uint GetWindowWidth() const;
	uint GetWindowHeight() const;
	float GetWinBrightness() const;
	uint GetDisplayRefreshRate();

	bool IsFullscreen() const;
	bool IsResizable() const;
	bool IsBorderless() const;
	bool IsFullscreenDesktop() const;

	// --- Save/Load ----
	void SaveStatus(json &file) const override;
	void LoadStatus(const json & file) override;

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
	uint				display_Width = 0; // To keep the original value
	uint				display_Height = 0; // To keep the original value

	// --- Win Flags ---
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool fullscreen_desktop = false;
};

#endif // __ModuleWindow_H__