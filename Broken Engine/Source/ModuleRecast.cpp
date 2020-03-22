#include "ModuleRecast.h"
#include "EngineApplication.h"

ModuleRecast::ModuleRecast(bool start_enabled) : Broken::Module(start_enabled) {
	name = "Recast";
}

ModuleRecast::~ModuleRecast() {
}

bool ModuleRecast::Init(Broken::json& config) {
	EngineApp->event_manager->AddListener(Broken::Event::EventType::GameObject_loaded, ONGameObjectAdded);
	return true;
}

void ModuleRecast::AddGO(Broken::GameObject* go) {
	if (go->navigationStatic)
		NavigationGameObjects.push_back(go);
}

void ModuleRecast::DeleteGO(Broken::GameObject* go) {
	for (std::vector<Broken::GameObject*>::iterator it = NavigationGameObjects.begin(); it != NavigationGameObjects.end(); ++it) {
		if (*it == go) {
			NavigationGameObjects.erase(it);
			break;
		}
	}
}

void ModuleRecast::ONGameObjectAdded(const Broken::Event& e) {
	if (e.go->navigationStatic)
		EngineApp->recast->NavigationGameObjects.push_back(e.go);
}
