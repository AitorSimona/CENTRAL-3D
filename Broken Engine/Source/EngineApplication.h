#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class ModuleEditorUI;

class EngineApplication : public Broken::Application {

public:

	EngineApplication();
	~EngineApplication();

public:
	ModuleEditorUI* editorui;
	
};

extern EngineApplication* EngineApp;

#endif