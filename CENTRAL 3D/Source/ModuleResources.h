#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Globals.h"
#include "Resource.h"

struct ResourceMeta
{
	Resource::ResourceType type = Resource::ResourceType::UNKNOWN;
	std::string original_file = "";
	std::string resource_name = "";
	uint UID = 0;

	bool Compare(const char* file, const char* name, Resource::ResourceType type)
	{
		return (original_file == file && resource_name == name && this->type == type);
	}

};

class ModuleResources : public Module
{
public:

	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(json file);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CreateMetaFromUID(uint UID, const char* filename);

	bool IsFileImported(const char* file);
	Resource* GetResource(uint UID);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);


	Resource* CreateResource(Resource::ResourceType type);
	//void AddResource(Resource* resource);
	void LoadResource(Resource* resource);
	uint DeleteResource(uint UID);
	void UnloadResource(uint UID);

private:
	// --- Resources in memory ---
	std::map<uint, Resource*> resources;

	// --- Resources in library ---
	std::map<uint, ResourceMeta> LoadedResources;
};

#endif