#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class EngineApplication : public Broken::Application {

public:

	EngineApplication();
	~EngineApplication();

public:
	class ModuleEditorUI* editorui;
	class ModuleRecast* recast;
	
};

extern EngineApplication* EngineApp;

#endif