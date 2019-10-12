#ifndef __MODULE_MESH_IMPORTER_H__
#define __MODULE_MESH_IMPORTER_H__

#include <vector>
#include "Module.h"
#include "Math.h"

class ComponentMesh;
class aiScene;

class ModuleMeshImporter : public Module
{
public:

	ModuleMeshImporter(bool start_enabled = true);
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
	void GetTextureIDFromSceneMaterial(const aiScene& scene, uint & texture_ID, std::string & directory);

private:
	std::vector<ComponentMesh*> meshes;
};

#endif
