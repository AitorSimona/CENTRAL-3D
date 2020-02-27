#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "BrokenEngineMain.h"


EngineApplication* EngineApp = NULL;

BrokenEngine::Application* BrokenEngine::CreateApplication() {
	return new EngineApplication();
}

EngineApplication::EngineApplication() {
	EngineApp = this;

	//We initiate our own modules
	editorui = new ModuleEditorUI();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

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
	AddModule(editorui);
	
	//// Scenes
	AddModule(scene_manager);
	
	////Gameplay (Scripting)
	AddModule(scripting);
	//// Renderer last!
	AddModule(renderer3D);
}

EngineApplication::~EngineApplication() {
}
