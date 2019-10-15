#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include <vector>
#include "Module.h"

class GameObject;
class ComponentMaterial;

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
	uint GetNumGameObjects() const;

	// --- Utilities ---
	void Draw() const;
	bool LoadFBX(const char* path);

private:
	std::vector<GameObject*> game_objects;
	std::vector<ComponentMaterial*> Materials;
};

#endif
