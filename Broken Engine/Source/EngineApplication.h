#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "BrokenEngine.h"

class EngineApplication : public Broken::Application {

public:

	EngineApplication();
	~EngineApplication();

	void SaveForBuild(const Broken::json& reference, const char* path) const;

public:
	class ModuleEditorUI* editorui;
	class ModuleRecast* recast;
	
};

extern EngineApplication* EngineApp;

#endif