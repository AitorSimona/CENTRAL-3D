#include "ModuleTimeManager.h"
#include "Application.h"
#include "ModuleGui.h"

ModuleTimeManager::ModuleTimeManager(bool start_enabled) : Module(start_enabled)
{
	CONSOLE_LOG("Initializing Time Manager");

	name = "TimeManager";
	ms_timer.Start();

	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60; // Get Display RR!!
	fps_counter = 0;
}

ModuleTimeManager::~ModuleTimeManager() {}

void ModuleTimeManager::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

void ModuleTimeManager::FinishUpdate()
{
	// Recap on framecount and fps
	++frames;
	++fps_counter;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = fps_counter;
		fps_counter = 0;
		fps_timer.Start();
	}

	last_frame_ms = ms_timer.Read();

	// cap fps
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay(capped_ms - last_frame_ms);

	// --- Send data to GUI- PanelSettings Historiograms
	App->gui->LogFPS((float)last_fps, (float)last_frame_ms);
}

uint ModuleTimeManager::GetMaxFramerate() const
{
	if (capped_ms > 0)
		return (uint)((1.0f / (float)capped_ms) * 1000.0f);
	else
		return 0;
}

void ModuleTimeManager::SetMaxFramerate(uint maxFramerate)
{
	if (maxFramerate > 0)
		capped_ms = 1000 / maxFramerate;
	else
		capped_ms = 0;
}

float ModuleTimeManager::GetDt() const
{
	return dt;
}
