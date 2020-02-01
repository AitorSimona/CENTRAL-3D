#ifndef __MODULE_EVENT_MANAGER_H__
#define __MODULE_EVENT_MANAGER_H__

#include "Module.h"

class ModuleEventManager : public Module
{
public:

	// --- Basic ---
	ModuleEventManager(bool start_enabled = true);
	~ModuleEventManager();

	bool Init(json file);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();
};

#endif