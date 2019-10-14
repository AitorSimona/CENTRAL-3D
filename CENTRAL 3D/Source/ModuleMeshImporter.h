#ifndef __MODULE_MESH_IMPORTER_H__
#define __MODULE_MESH_IMPORTER_H__

#include <vector>
#include "Module.h"
#include "Math.h"

class aiScene;
class GameObject;
class ComponentMaterial;

class ModuleMeshImporter : public Module
{
public:
	// --- Basic ---
	ModuleMeshImporter(bool start_enabled = true);
	~ModuleMeshImporter();
	bool Init(json file);
	bool Start();
	bool CleanUp();

	// --- Getters ---
	uint GetNumMeshes() const;

	// --- Utilities ---
	void Draw() const;
	bool LoadFBX(const char* path);

private:
	std::vector<GameObject*> game_objects;
	std::vector<ComponentMaterial*> Materials;
};

#endif
