#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class EngineApplication : public BrokenEngine::Application {

	EngineApplication();
	~EngineApplication();
	
};

BrokenEngine::Application* CreateApplication();

#endif