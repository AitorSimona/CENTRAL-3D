#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include <list>

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

	void SetMaxFramerate(uint maxFramerate);
	uint GetMaxFramerate() const;

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
	uint				capped_ms;
	int					last_frame_ms;

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