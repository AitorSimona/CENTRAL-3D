#ifndef __MODULE_TIME_MANAGER_H__
#define __MODULE_TIME_MANAGER_H__

#include "Module.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "BrokenCore.h"


BE_BEGIN_NAMESPACE
class BROKEN_API ModuleTimeManager : public Module {
public:

	ModuleTimeManager(bool start_enabled = true);
	~ModuleTimeManager();

	void PrepareUpdate();
	void FinishUpdate();

	// --- Getters ---
	float GetGameDt() const;
	float GetRealTimeDt()const;
	uint GetMaxFramerate() const;
	float GetTimeScale() const;
	float GetGameplayTimePassed() const;
	float GetFPS() const;
	uint GetLastFrameMs() const;
	// --- Setters ---
	void SetMaxFramerate(uint maxFramerate);
	void SetTimeScale(float scale);

	float time = 0.0f;
	bool gamePaused = false;

private:

	Timer	Realtime_clock;
	Timer	Gametime_clock;
	float	Time_scale = 1.0f;


	Timer	fps_timer;
	float	game_dt = 0.0f;
	float	realtime_dt = 0.0f;
	float	gametime_passed = 0.0f;
	Uint32	frame_count;
	int		fps_counter;
	int		last_fps;
	uint	capped_ms;
	uint	last_frame_ms;
};
BE_END_NAMESPACE
#endif