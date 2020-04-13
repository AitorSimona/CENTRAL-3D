#include "BrokenCore.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "SDL/include/SDL.h"

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#include "mmgr/mmgr.h"

using namespace Broken;

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled) {
	name = "Window";
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow() {
}

// Called before render is available
bool ModuleWindow::Init(json& file) {
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("SDL_Init Video success");

		LoadStatus(file);

		// --- Get Display Data ---
		SDL_DisplayMode display;
		SDL_GetCurrentDisplayMode(0, &display);
		display_Width = display.w;
		display_Height = display.h;

		// --- Assign Display Specific values to code vars ---
		screen_width = uint(display.w * 0.75f);
		screen_height = uint(display.h * 0.75f);

		RefreshRate = display.refresh_rate;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // deprecated functions are enabled --> SDL_GL_CONTEXT_PROFILE_CORE to disable || SDL_GL_CONTEXT_PROFILE_COMPATIBILITY to enable
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		if (fullscreen == true) {
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (resizable == true) {
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (borderless == true) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (fullscreen_desktop == true) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}


		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);

		if (window == NULL) {
			ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else {
			ENGINE_AND_SYSTEM_CONSOLE_LOG("Successfully created Window: %s", App->GetAppName());
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

bool ModuleWindow::Start() {
	SetWinBrightness(1.0f);

	// Force to trigger a chain of events to refresh aspect ratios	
	SDL_SetWindowSize(window, screen_width, screen_height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp() {
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != NULL) {
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title) {
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetWindowWidth(uint width) {
	screen_width = width;

	if (width < 950)
		screen_width = 950;

	UpdateWindowSize();
}

void ModuleWindow::SetWindowHeight(uint height) {
	screen_height = height;

	if (height < 320)
		screen_height = 320;

	UpdateWindowSize();
}

void ModuleWindow::SetMouseFocus(bool focus) {
	mouse_focus = focus;
}

uint ModuleWindow::GetWindowWidth() const {
	return screen_width;
}

uint ModuleWindow::GetWindowHeight() const {
	return screen_height;
}

uint ModuleWindow::GetDisplayRefreshRate() {
	uint refreshRate = 0;

	SDL_DisplayMode desktopDisplay;
	if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
		refreshRate = desktopDisplay.refresh_rate;
	else
		ENGINE_CONSOLE_LOG("|[error]: SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

	RefreshRate = refreshRate;

	return refreshRate;
}

void ModuleWindow::GetWinMaxMinSize(uint& min_width, uint& min_height, uint& max_width, uint& max_height) const {
	// --- Function used to set bounds to window resize by user ---

	max_width = display_Width;
	max_height = display_Height;
	min_width = 640;
	min_height = 480;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
		ENGINE_CONSOLE_LOG("|[error]: SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}
	else {
		max_width = dm.w;
		max_height = dm.h;
	}
}

const json& ModuleWindow::SaveStatus() const
{
	
	static json m_config;

	m_config["borderless"] = borderless;
	m_config["fullscreen"] = fullscreen;
	m_config["fullscreenDesktop"] = fullscreen_desktop;
	m_config["resizable"] = resizable;
	m_config["height"] = screen_height;
	m_config["width"] = screen_width;
	m_config["height"] = screen_height;
	m_config["width"] = screen_width;
	m_config["sceneY"] = 640;
	m_config["sceneX"] = 480;

	return m_config;

}

void ModuleWindow::LoadStatus(const json& file)
{
	screen_width = file["Window"]["sceneX"].is_null() ? 640 : file["Window"]["sceneX"].get<uint>();
	screen_height = file["Window"]["sceneY"].is_null() ? 480 : file["Window"]["sceneY"].get<uint>();
}

void ModuleWindow::SetFullscreen(bool value) {
	if (value != fullscreen) {
		fullscreen = value;
		if (fullscreen == true) {
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
				ENGINE_CONSOLE_LOG("|[error]: Could not switch to fullscreen: %s\n", SDL_GetError());
			fullscreen_desktop = false;

		}
		else {
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				ENGINE_CONSOLE_LOG("|[error]: Could not switch to windowed: %s\n", SDL_GetError());

			//  --- To keep a default window size on deactivation ---
			SetWindowWidth(uint(display_Width * 0.75f));
			SetWindowHeight(uint(display_Height * 0.75f));
		}
	}

}

void ModuleWindow::SetResizable(bool value) {
	// --- Restart needed to apply ---
	resizable = value;
}

void ModuleWindow::SetBorderless(bool value) {
	if (value != borderless && fullscreen == false && fullscreen_desktop == false) {
		borderless = value;
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
	}
}

void ModuleWindow::SetFullscreenDesktop(bool value) {
	if (value != fullscreen_desktop) {
		fullscreen_desktop = value;
		if (fullscreen_desktop == true) {
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
				ENGINE_CONSOLE_LOG("|[error]: Could not switch to fullscreen desktop: %s\n", SDL_GetError());
			fullscreen = false;

		}
		else {
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				ENGINE_CONSOLE_LOG("|[error]: Could not switch to windowed: %s\n", SDL_GetError());

			//  --- To keep a default window size on deactivation ---
			SetWindowWidth(uint(display_Width * 0.75f));
			SetWindowHeight(uint(display_Height * 0.75f));
		}
	}
}

bool ModuleWindow::IsFullscreen() const {
	return fullscreen;
}

bool ModuleWindow::IsResizable() const {
	return resizable;
}

bool ModuleWindow::IsBorderless() const {
	return borderless;
}

bool ModuleWindow::IsFullscreenDesktop() const {
	return fullscreen_desktop;
}

bool ModuleWindow::isMouseFocused() const {
	return mouse_focus;
}

void ModuleWindow::SetWinBrightness(float value) {
	CAP(value); // Force values from 0 to 1
	if (SDL_SetWindowBrightness(window, value) != 0)
		ENGINE_CONSOLE_LOG("|[error]: Could not change window brightness: %s\n", SDL_GetError());
}

void ModuleWindow::SetWinTitle(const char* name) {
	SDL_SetWindowTitle(window, name);
}

float ModuleWindow::GetWinBrightness() const {
	return SDL_GetWindowBrightness(window);
}

void ModuleWindow::UpdateWindowSize() const {
	SDL_SetWindowSize(window, screen_width, screen_height);
	App->renderer3D->OnResize(screen_width, screen_height);
}
