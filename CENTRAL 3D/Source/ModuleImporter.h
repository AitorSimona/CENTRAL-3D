#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Module.h"

class ModuleImporter : public Module
{
public:
	// --- Basic ---
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();
	bool Init(json file);
	bool Start();
	bool CleanUp();

	// --- Getters ---

	// --- Utilities ---

private:

	void ImportAssets(const char * directory, std::vector<std::string>& filters);
};

#endif
