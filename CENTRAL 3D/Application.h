#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include <list>
#include <vector>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleHardware.h"

#include "PerfTimer.h"

class Application
{
public:
	const char* GetAppName() const;

	void SetCapFrames(bool capFrames);
	bool GetCapFrames() const;
	void SetMaxFramerate(uint maxFramerate);
	uint GetMaxFramerate() const;

	void AddFramerateToTrack(float fps);
	std::vector<float> GetFramerateTrack() const;
	void AddMsToTrack(float ms);
	std::vector<float> GetMsTrack() const;

public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui*			gui;
	ModuleHardware* hardware;

private:

	std::list<Module*> list_modules;
	const char* appName = "CENTRAL 3D";

	Timer				ms_timer;
	Timer				fps_timer;
	float				dt = 0;

	Uint32				frames;
	int					fps_counter;
	int					last_fps;
	int					capped_ms;
	int					last_frame_ms;
	bool				capFrames = false;
	uint				maxFramerate = 0;
	std::vector<float>	fpsTrack;
	std::vector<float>	msTrack;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif