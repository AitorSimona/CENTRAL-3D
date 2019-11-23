#ifndef __MODULE_TIME_MANAGER_H__
#define __MODULE_TIME_MANAGER_H__

#include "Module.h"
#include "Timer.h"
#include "PerfTimer.h"


class ModuleTimeManager : public Module
{
public:

	ModuleTimeManager(bool start_enabled = true);
	~ModuleTimeManager();

	void PrepareUpdate();
	void FinishUpdate();

	// --- Getters ---
	float GetDt() const;
	uint GetMaxFramerate() const;

	// --- Setters ---
	void SetMaxFramerate(uint maxFramerate);


private:

	Timer				ms_timer;
	Timer				fps_timer;
	float				dt = 0;
	Uint32				frames;
	int					fps_counter;
	int					last_fps;
	uint				capped_ms;
	uint				last_frame_ms;
};

#endif