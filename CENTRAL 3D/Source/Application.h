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

enum AppState
{
	PLAY = 0,
	TO_PLAY,

	EDITOR,
	TO_EDITOR,

	PAUSE,
	TO_PAUSE,

	STEP,
};

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleHardware;
class ModuleFileSystem;
class ModuleImporter;
class ModuleTextures;
class ModuleSceneManager;
class ModuleResources;
class ModuleTimeManager;

class Application
{
public:

	// --- Getters ---
	const char * GetAppName() const;
	const char* GetOrganizationName() const;
	json GetDefaultConfig() const;
	std::vector<std::string>& GetLogs();
	LCG& GetRandom();
	JSONLoader* GetJLoader();
	AppState& GetAppState();

	// --- Setters ---
	void SetAppName(const char* name);
	void SetOrganizationName(const char* name);
	void Log(const char* entry);
	void ClearLogsFromConsole();

public:

	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleGui*			gui = nullptr;
	ModuleHardware* hardware = nullptr;
	ModuleFileSystem* fs = nullptr;
	ModuleImporter* importer = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleSceneManager* scene_manager = nullptr;
	ModuleResources* resources = nullptr;
	ModuleTimeManager* time = nullptr;

private:

	std::list<Module*> list_modules;

	JSONLoader			JLoader;
	std::string			appName;
	std::string			orgName;
	std::string			configpath;

	LCG*		  RandomNumber = nullptr;

	std::string			log;
	std::vector<std::string> logs;

	AppState EngineState = AppState::EDITOR;


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