#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleHardware.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

Application::Application()
{
	appName = "";
	log = "Application Logs:";
	configpath = "Settings/EditorConfig.json";
	RandomNumber = new math::LCG();

	time = new ModuleTimeManager(true);
	hardware = new ModuleHardware(true);
	fs =  new ModuleFileSystem(true,ASSETS_FOLDER);
	window = new ModuleWindow(true);
	scene_manager = new ModuleSceneManager(true);
	importer = new ModuleImporter(true);
	input = new ModuleInput(true);
	renderer3D = new ModuleRenderer3D(true);
	camera = new ModuleCamera3D(true);
	gui = new ModuleGui(true);
	textures = new ModuleTextures(true);
	resources = new ModuleResources(true);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(time);


	AddModule(textures);
	AddModule(hardware);
	AddModule(fs);
	AddModule(importer);
	AddModule(resources);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);

	// Scenes
	AddModule(scene_manager);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend())
	{
		delete *item;
		item++;
	}

	if (RandomNumber)
	{
		delete RandomNumber;
		RandomNumber = nullptr;
	}
}

bool Application::Init()
{
	bool ret = true;

	// --- Load App data from JSON files ---
	json config = JLoader.Load(configpath.data());

	// --- Create Config with default values if load fails ---
	if (config.is_null())
	{
		config = GetDefaultConfig();
	}

	// --- Reading App Name/ Org Name from json file ---
	std::string tmp = config["Application"]["Title"];
	appName = tmp;

	std::string tmp2 = config["Application"]["Organization"];
	orgName = tmp2;

	// Call Init() in all modules

	std::list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init(config);
		item++;
	}

	// After all Init calls we call Start() in all modules
	CONSOLE_LOG("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item++;
	}

	time->SetMaxFramerate(App->window->GetDisplayRefreshRate());

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	time->PrepareUpdate();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	time->FinishUpdate();
}

void Application::SaveAllStatus()
{
	// --- Create Config with default values ---
	json config = GetDefaultConfig();

	std::string tmp = appName;
	config["Application"]["Title"] = tmp;
	std::string tmp2 = orgName;
	config["Application"]["Organization"] = tmp2;

	// --- Call Save of all modules ---

	std::list<Module*>::const_iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->SaveStatus(config);
		item++;
	}

	JLoader.Save(configpath.data(), config);
}

void Application::LoadAllStatus(json & file)
{
	// --- This function is not called at startup, but later if needed ---

	// --- Reading App name from json file ---
	std::string tmp = file["Application"]["Title"];
	appName = tmp;

	std::string tmp2 = file["Application"]["Organization"];
	orgName = tmp2;

	// --- Call Load of all modules ---

	json config = JLoader.Load(configpath.data());

	std::list<Module*>::const_iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->LoadStatus(config);
		item++;
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::const_iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(time->GetRealTimeDt());
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(time->GetRealTimeDt());
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(time->GetRealTimeDt());
		item++;
	}

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	// --- Save all Status --- TODO: Should be called by user
	SaveAllStatus();

	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}


const char * Application::GetAppName() const
{
	return appName.data();
}

void Application::SetAppName(const char* name) 
{
	appName.assign(name);
	App->window->SetWinTitle(appName.data());
}

void Application::SetOrganizationName(const char* name)
{
	orgName = name;
}

void Application::Log(const char * entry)
{	
	// --- Append all logs to a string so we can print them on console --- 
	log.append(entry);

	std::string to_add = entry;
	logs.push_back(to_add);
}

void Application::ClearLogsFromConsole()
{
	logs.erase(logs.begin(), logs.end());
	logs.clear();
}

const char* Application::GetOrganizationName() const
{
	return orgName.data();
}

json Application::GetDefaultConfig() const
{
	// --- Create Config with default values ---
	json config = {
		{"Application", {

		}},

		{"GUI", {

		}},

		{"Window", {
			{"width", 1024},
			{"height", 720},
			{"fullscreen", false},
			{"resizable", true},
			{"borderless", false},
			{"fullscreenDesktop", false}
		}},

		{"Input", {

		}},

		{"Renderer3D", {
			{"VSync", true}
		}},
	};

	return config;
}

std::vector<std::string>& Application::GetLogs()
{
	return logs;
}

// ---------------------------------------------
LCG & Application::GetRandom()
{
	return *RandomNumber;
}

JSONLoader * Application::GetJLoader()
{
	return &JLoader;
}

AppState & Application::GetAppState()
{
	return EngineState;
}


