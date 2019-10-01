#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include <list>
#include <string>

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

#include "JSONLoader.h"


class Application
{
public:

	void SetMaxFramerate(uint maxFramerate);
	uint GetMaxFramerate() const;
	const char * GetAppName() const;
	void SetAppName(const char* name);
	void SetOrganizationName(const char* name);
	const char* GetOrganizationName() const;

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

	Timer				ms_timer;
	Timer				fps_timer;
	float				dt = 0;

	Uint32				frames;
	int					fps_counter;
	int					last_fps;
	uint				capped_ms;
	uint					last_frame_ms;

	JSONLoader			JLoader;
	std::string			appName;
	std::string			orgName;

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
	void SaveAllStatus();
	void LoadAllStatus(json & file);
};

extern Application* App;

#endif