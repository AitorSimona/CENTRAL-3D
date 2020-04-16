#include "GameApplication.h"
#include "ModuleGameManager.h"
#include "ModuleEventManager.h"
#include "ModuleInput.h"
#include "ModuleTimeManager.h"
#include "ModuleHardware.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleScripting.h"
#include "ModuleThreading.h"
#include "ModuleUI.h"
#include "ModulePhysics.h"
#include "ModuleParticles.h"
#include "ModuleAudio.h"
#include "ModuleDetour.h"
#include "ModuleSelection.h"
#include "BrokenEngineMain.h"
#include "BrokenEngineMain.h"

GameApplication* GameApp = NULL;

Broken::Application* Broken::CreateApplication() {
	return new GameApplication();
}

GameApplication::GameApplication() {
	GameApp = this;
	isGame = true;
	SetConfigPath("Settings/GameConfig.json");

	//We initiate our own modules
	game_manager = new ModuleGameManager();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	gui->SetEnabled(false);

	// Main Modules
	AddModule(threading);
	AddModule(fs);
	AddModule(event_manager);
	AddModule(input);
	AddModule(time);


	AddModule(textures);
	AddModule(hardware);

	AddModule(resources);
	AddModule(window);
	AddModule(camera);
	AddModule(gui);
	AddModule(ui_system);

	// Scenes
	AddModule(scene_manager);
	//Module to set up and manage the game
	AddModule(game_manager);

	//Recast
	AddModule(detour);

	//Physics and particles
	AddModule(physics);
	AddModule(particles);

	//Audio
	AddModule(audio);

	//Gameplay (Scripting)
	AddModule(scripting);

	// Renderer last!
	AddModule(renderer3D);
}

GameApplication::~GameApplication() {
}
