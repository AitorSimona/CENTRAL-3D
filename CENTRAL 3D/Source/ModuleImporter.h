#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Module.h"

class ImporterScene;

class ModuleImporter : public Module
{
public:
	// --- Basic ---
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();
	bool Init(json file);
	bool Start();
	bool CleanUp();

	// --- Utilities ---
	bool LoadFBX(const char* path);

private:
	ImporterScene* IScene = nullptr;

};

#endif
