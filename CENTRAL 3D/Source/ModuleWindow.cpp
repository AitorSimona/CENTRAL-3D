#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleTimeManager.h"

#include "mmgr/mmgr.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

#define RESIZE_BORDER 5

// https://fossies.org/linux/SDL2/test/testhittesting.c

SDL_Rect drag_areas[] = 
{
    { 0, 0, 1920, 100 } // default
};

const uint numDragAreas = SDL_arraysize(drag_areas);

static SDL_HitTestResult hitTest(SDL_Window *window, const SDL_Point *pt, void *data)
{
     int i;
     int w, h;
 
     for (i = 0; i < numDragAreas; i++)
	 {
         if (SDL_PointInRect(pt, &drag_areas[i]))
		 {
             SDL_Log("HIT-TEST: DRAGGABLE\n");
             return SDL_HITTEST_DRAGGABLE;
         }
     }
 
     SDL_GetWindowSize(window, &w, &h);
 
     #define REPORT_RESIZE_HIT(name) { \
         SDL_Log("HIT-TEST: RESIZE_" #name "\n"); \
         return SDL_HITTEST_RESIZE_##name; \
     }
 
     if (pt->x < RESIZE_BORDER && pt->y < RESIZE_BORDER) {
         REPORT_RESIZE_HIT(TOPLEFT);
     } else if (pt->x > RESIZE_BORDER && pt->x < w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
         REPORT_RESIZE_HIT(TOP);
     } else if (pt->x > w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
         REPORT_RESIZE_HIT(TOPRIGHT);
     } else if (pt->x > w - RESIZE_BORDER && pt->y > RESIZE_BORDER && pt->y < h - RESIZE_BORDER) {
         REPORT_RESIZE_HIT(RIGHT);
     } else if (pt->x > w - RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
         REPORT_RESIZE_HIT(BOTTOMRIGHT);
     } else if (pt->x < w - RESIZE_BORDER && pt->x > RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
         REPORT_RESIZE_HIT(BOTTOM);
     } else if (pt->x < RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
         REPORT_RESIZE_HIT(BOTTOMLEFT);
     } else if (pt->x < RESIZE_BORDER && pt->y < h - RESIZE_BORDER && pt->y > RESIZE_BORDER) {
         REPORT_RESIZE_HIT(LEFT);
     }
 
     SDL_Log("HIT-TEST: NORMAL\n");
     return SDL_HITTEST_NORMAL;
}

// Called before render is available
bool ModuleWindow::Init(json file)
{
	CONSOLE_LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		CONSOLE_LOG("|[error]: SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		CONSOLE_LOG("SDL_Init Video success");

		// --- Get Display Data ---
		SDL_DisplayMode display;
		SDL_GetCurrentDisplayMode(0, &display);
		display_Width = display.w;
		display_Height = display.h;

		// --- Assign Display Specific values to code vars ---
		screen_width = uint(display.w * 0.75f);
		screen_height = uint(display.h * 0.75f);
		RefreshRate = display.refresh_rate;
		App->time->CapMs(1 / RefreshRate * 1000);
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // deprecated functions are disabled
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}


		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);

		if(window == NULL)
		{
			CONSOLE_LOG("|[error]: Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			CONSOLE_LOG("Successfully created Window: %s", App->GetAppName());
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			if (borderless)
			{
				// --- Set hit test drag areas ---
				drag_areas[0] = SDL_Rect{ 300,0,(int)(screen_width - 400), 20 };

				// --- Set SDL hit test callback ---
				if (SDL_SetWindowHitTest(window, hitTest, NULL) == -1)
				{
					SDL_Log("Enabling hit-testing failed!\n");
					SDL_Quit();
					return 1;
				}
			}
		}

	}

	return ret;
}

bool ModuleWindow::Start()
{
	SetWinBrightness(1.0f);

	// Force to trigger a chain of events to refresh aspect ratios	
	SDL_SetWindowSize(window, screen_width, screen_height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	CONSOLE_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetWindowWidth(uint width)
{
		screen_width = width;

		if (width < 950)
			screen_width = 950;

		UpdateWindowSize();
}

void ModuleWindow::SetWindowHeight(uint height)
{
		screen_height = height;

		if (height < 320)
			screen_height = 320;

		UpdateWindowSize();
}

void ModuleWindow::MinimizeWindow()
{
	SDL_MinimizeWindow(window);
	maximized = false;
}

void ModuleWindow::MaximizeWindow()
{
	SDL_MaximizeWindow(window);
	maximized = true;
}

void ModuleWindow::RestoreWindow()
{
	SDL_RestoreWindow(window);
	maximized = false;
}

bool ModuleWindow::IsWindowMaximized()
{
	return maximized;
}

uint ModuleWindow::GetWindowWidth() const
{
	return screen_width;
}

uint ModuleWindow::GetWindowHeight() const
{
	return screen_height;
}

uint ModuleWindow::GetDisplayWidth() const
{
	return display_Width;
}

uint ModuleWindow::GetDisplayHeight() const
{
	return display_Height;
}

uint ModuleWindow::GetDisplayRefreshRate() 
{
	uint refreshRate = 0;

	SDL_DisplayMode desktopDisplay;
	if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
		refreshRate = desktopDisplay.refresh_rate;
	else
		CONSOLE_LOG("|[error]: SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

	RefreshRate = refreshRate;

	return refreshRate;
}

void ModuleWindow::GetWinMaxMinSize(uint & min_width, uint & min_height, uint & max_width, uint & max_height) const
{
	// --- Function used to set bounds to window resize by user ---

	max_width = display_Width;
	max_height = display_Height;
	min_width = 640;
	min_height = 480;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		CONSOLE_LOG("|[error]: SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}
	else
	{
		max_width = dm.w;
		max_height = dm.h;
	}
}

void ModuleWindow::SaveStatus(json & file) const
{

}

void ModuleWindow::LoadStatus(const json & file)
{
	
}

void ModuleWindow::SetFullscreen(bool value)
{
	if (value != fullscreen)
	{
		fullscreen = value;
		if (fullscreen == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
				CONSOLE_LOG("|[error]: Could not switch to fullscreen: %s\n", SDL_GetError());
			fullscreen_desktop = false;

		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				CONSOLE_LOG("|[error]: Could not switch to windowed: %s\n", SDL_GetError());

			//  --- To keep a default window size on deactivation ---
			SetWindowWidth(uint(display_Width *0.75f));
			SetWindowHeight(uint(display_Height*0.75f));
		}
	}

}

void ModuleWindow::SetResizable(bool value)
{ 
	// --- Restart needed to apply ---
	resizable = value;
}

void ModuleWindow::SetBorderless(bool value)
{
	if (value != borderless && fullscreen == false && fullscreen_desktop == false)
	{
		borderless = value;
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
	}
}

void ModuleWindow::SetFullscreenDesktop(bool value)
{
	if (value != fullscreen_desktop)
	{
		fullscreen_desktop = value;
		if (fullscreen_desktop == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
				CONSOLE_LOG("|[error]: Could not switch to fullscreen desktop: %s\n", SDL_GetError());
			fullscreen = false;

		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				CONSOLE_LOG("|[error]: Could not switch to windowed: %s\n", SDL_GetError());

			//  --- To keep a default window size on deactivation ---
			SetWindowWidth(uint(display_Width *0.75f));
			SetWindowHeight(uint(display_Height*0.75f));
		}
	}
}

bool ModuleWindow::IsFullscreen() const
{
	return fullscreen;
}

bool ModuleWindow::IsResizable() const
{
	return resizable;
}

bool ModuleWindow::IsBorderless() const
{
	return borderless;
}

bool ModuleWindow::IsFullscreenDesktop() const
{
	return fullscreen_desktop;
}

void ModuleWindow::SetWinBrightness(float value)
{
	CAP(value); // Force values from 0 to 1
	if (SDL_SetWindowBrightness(window, value) != 0)
		CONSOLE_LOG("|[error]: Could not change window brightness: %s\n", SDL_GetError());
}

void ModuleWindow::SetWinTitle(const char * name)
{
	SDL_SetWindowTitle(window,name);
}

float ModuleWindow::GetWinBrightness() const
{
	return SDL_GetWindowBrightness(window);
}

void ModuleWindow::UpdateWindowSize() const
{
	SDL_SetWindowSize(window, screen_width, screen_height);
	App->renderer3D->OnResize(screen_width, screen_height);

	// --- Set hit test drag areas ---
	drag_areas[0] = SDL_Rect{ 300,0,(int)(screen_width - 400), 20 };
}