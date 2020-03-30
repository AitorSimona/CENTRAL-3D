#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class ModuleEditorUI;

class EngineApplication : public Broken::Application {

public:

	EngineApplication();
	~EngineApplication();

	void SaveForBuild(const Broken::json& reference, const char* path) const;

public:
	ModuleEditorUI* editorui;
	
};

extern EngineApplication* EngineApp;

#endif