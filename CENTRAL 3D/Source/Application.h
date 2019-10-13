#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "Globals.h"
#include <list>
#include <string>
#include <vector>
#include "Timer.h"
#include "PerfTimer.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"
#include "JSONLoader.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleHardware;
class ModuleFileSystem;
class ModuleMeshImporter;
class ModuleTextures;

class Application
{
public:

	// --- Getters ---
	uint GetMaxFramerate() const;
	const char * GetAppName() const;
	const char* GetOrganizationName() const;
	json GetDefaultConfig() const;
	std::vector<std::string>& GetLogs();
	LCG& GetRandom();

	// --- Setters ---
	void SetMaxFramerate(uint maxFramerate);
	void SetAppName(const char* name);
	void SetOrganizationName(const char* name);

	void Log(const char* entry);
	void ClearLogsFromConsole();



public:

	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleSceneIntro* scene_intro = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleGui*			gui = nullptr;
	ModuleHardware* hardware = nullptr;
	ModuleFileSystem* fs = nullptr;
	ModuleMeshImporter* meshImporter = nullptr;
	ModuleTextures* textures = nullptr;

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
	std::vector<std::string> logs;

	LCG*		  RandomNumber = nullptr;


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