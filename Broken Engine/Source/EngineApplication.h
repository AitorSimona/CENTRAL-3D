#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class ModuleGui;

class EngineApplication : public BrokenEngine::Application {

public:

	EngineApplication();
	~EngineApplication();
	
};

extern EngineApplication* App;
BrokenEngine::Application* CreateApplication();

#endif