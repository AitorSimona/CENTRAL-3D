#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "BrokenCore.h"
#include <list>
#include <string>
#include <vector>
#include "RandomGenerator.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"
#include "JSONLoader.h"

#define MAX_CONSOLE_LOGS 1000

BE_BEGIN_NAMESPACE
enum AppState {
	PLAY = 0,
	TO_PLAY,

	EDITOR,
	TO_EDITOR,

	PAUSE,
	TO_PAUSE,

	STEP,
};

class Module;

class BROKEN_API Application {
public:

	// --- Getters ---
	const char* GetAppName() const;
	const char* GetOrganizationName() const;
	json GetDefaultConfig() const;
	void GetDefaultGameConfig(json& config) const;
	json& GetConfigFile();
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

	class ModuleWindow*				window = nullptr;
	class ModuleInput*				input = nullptr;
	class ModuleRenderer3D*			renderer3D = nullptr;
	class ModuleCamera3D*			camera = nullptr;
	class ModuleGui*				gui = nullptr;
	class ModuleHardware*			hardware = nullptr;
	class ModuleFileSystem*			fs = nullptr;
	class ModuleTextures*			textures = nullptr;
	class ModuleSceneManager*		scene_manager = nullptr;
	class ModuleResourceManager*	resources = nullptr;
	class ModuleTimeManager*		time = nullptr;
	class ModuleEventManager*		event_manager = nullptr;
	class ModuleScripting*			scripting = nullptr;
	class ModuleThreading*			threading = nullptr;
	class ModuleUI*					ui_system = nullptr;
	class ModulePhysics*			physics = nullptr;
	class ModuleParticles*			particles = nullptr;
	class ModuleAudio*				audio = nullptr;
	class ModuleDetour*				detour = nullptr;


	bool isGame = false;

	//Random Number Generator
	RNGen RandomNumberGenerator;

private:

	std::list<Module*> list_modules;

	JSONLoader			JLoader;
	std::string			appName;
	std::string			orgName;
	std::string			configpath;
	json				tempjson;

	LCG*				RandomNumber = nullptr;

	std::string			log;
	std::vector<std::string> logs;

	AppState EngineState = AppState::EDITOR;


public:

	Application();
	virtual ~Application();

	bool Init();
	update_status Update();
	bool CleanUp();


protected:
	void AddModule(Module* mod);
	void SetConfigPath(const char* path);

private:
	void PrepareUpdate();
	void FinishUpdate();
	void SaveAllStatus();
	void LoadAllStatus(json& file);
};

extern BROKEN_API Application* App;
Application* CreateApplication();
BE_END_NAMESPACE


#endif
