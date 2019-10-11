#ifndef __MODULE_MESH_IMPORTER_H__
#define __MODULE_MESH_IMPORTER_H__

#include <vector>
#include "Module.h"
#include "Math.h"

class ComponentMesh;

class ModuleMeshImporter : public Module
{
public:

	ModuleMeshImporter(Application* app, bool start_enabled = true);
	~ModuleMeshImporter();

	bool Init(json file);
	bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	void Draw();
	bool CleanUp();
	bool LoadFBX(const char* path);

	uint GetNumMeshes() const;

private:
	std::vector<ComponentMesh*> meshes;

	uint ImageName = 0;
};

#endif
