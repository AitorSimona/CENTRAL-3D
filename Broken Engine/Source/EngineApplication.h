#ifndef __ENGINEAPPLICATION_H__
#define __ENGINEAPPLICATION_H__

#include "Application.h"

class EngineApplication : public Broken::Application {

public:

	EngineApplication();
	~EngineApplication();

	void SaveForBuild(const Broken::json& reference, const char* path) const;
	void GetDefaultGameConfig(Broken::json& config) const;

public:
	class ModuleEditorUI* editorui;
	class ModuleRecast* recast;
	
};

extern EngineApplication* EngineApp;

#endif