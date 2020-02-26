#include "EngineApplication.h"

BrokenEngine::Application* CreateApplication() {
	return new EngineApplication();
}

EngineApplication::EngineApplication() {
}

EngineApplication::~EngineApplication() {
}
