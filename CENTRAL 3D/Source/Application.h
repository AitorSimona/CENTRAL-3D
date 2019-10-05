#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "Globals.h"
#include <list>
#include <string>
#include "Timer.h"
#include "PerfTimer.h"

#include "JSONLoader.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleHardware;

class Application
{
public:

	// --- Getters ---
	uint GetMaxFramerate() const;
	const char * GetAppName() const;
	const char* GetOrganizationName() const;
	json GetDefaultConfig() const;

	// --- Setters ---
	void SetMaxFramerate(uint maxFramerate);
	void SetAppName(const char* name);
	void SetOrganizationName(const char* name);

	void Log(const char* entry);


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
	std::string			configpath;
	std::string			log;

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