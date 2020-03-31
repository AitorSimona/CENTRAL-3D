#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "ModuleRecast.h"
#include "BrokenEngineMain.h"


EngineApplication* EngineApp = NULL;

Broken::Application* Broken::CreateApplication() {
	return new EngineApplication();
}

EngineApplication::EngineApplication() {
	EngineApp = this;
	SetConfigPath("Settings/EditorConfig.json");

	//We initiate our own modules
	editorui = new ModuleEditorUI();
	recast = new ModuleRecast();

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
	AddModule(editorui);
	AddModule(gui);
	AddModule(ui_system);
	
	// Scenes
	AddModule(scene_manager);
	AddModule(selection);

	// Recast
	AddModule(recast);
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

EngineApplication::~EngineApplication() {
}

void EngineApplication::SaveForBuild(const Broken::json& reference, const char* path) const {
	static Broken::json config = reference;

	Broken::json node;
	for (std::list<Broken::Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); ++item) {
		if ((*item) == editorui)
			continue;

		if ((*item) == window)
			continue;

		if ((*item)->isEnabled()) {
			node = (*item)->SaveStatus();
			if (node.begin() != node.end())
				config[(*item)->GetName()] = node;
		}
	}

	EngineApp->GetJLoader()->Save(path, config);
}
