#include "EngineApplication.h"
#include "ModuleGui.h"

BrokenEngine::Application* CreateApplication() {
	return new EngineApplication();
}

EngineApplication::EngineApplication() {
	gui = new ModuleGui();
}

EngineApplication::~EngineApplication() {
}
