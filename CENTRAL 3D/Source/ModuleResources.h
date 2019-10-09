#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include <vector>
#include "Module.h"
#include "Math.h"

class ResourceMesh;

class ModuleResources : public Module
{
public:

	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	bool Init(json file);
	bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	void Draw();
	bool CleanUp();
	bool LoadFBX(const char* path);

private:
	std::vector<ResourceMesh*> meshes;
};

#endif
