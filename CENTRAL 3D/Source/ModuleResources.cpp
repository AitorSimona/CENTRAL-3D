#include "ModuleResources.h"
#include "Application.h"
#include "ModuleFileSystem.h"

ModuleResources::ModuleResources(bool start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(json file)
{
	return true;
}

bool ModuleResources::Start()
{
	return true;
}

update_status ModuleResources::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	return true;
}

Resource * ModuleResources::GetResource(uint UID)
{
	Resource* ret = nullptr;

	// --- If resource is loaded into memory, return pointer to it, else load it ---

	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end())
		ret = it->second;
	else
	{
		// --- If resource is not in memory, search in library ---
		std::map<uint, ResourceMeta>::iterator it = LoadedResources.find(UID);

		if (it != LoadedResources.end())
		{
			switch (it->second.type)
			{
			case Resource::ResourceType::MESH:

				break;

			case Resource::ResourceType::TEXTURE:

				break;

			case Resource::ResourceType::MATERIAL:

				break;

			}
		}

		if (ret)
		{
			ret->original_file = it->second.original_file;
			ret->name = it->second.resource_name;
			LOG("Loaded Resource: %s", ret->name);
		}
	}

	return ret;
}

Resource::ResourceType ModuleResources::GetResourceTypeFromPath(const char * path)
{
	std::string extension = "";
	App->fs->SplitFilePath(path, nullptr, nullptr, &extension);

	Resource::ResourceType type = Resource::ResourceType::UNKNOWN;

	if (extension == ".mesh")
		type = Resource::ResourceType::MESH;
	else if (extension == ".dds")
		type = Resource::ResourceType::TEXTURE;

	return type;
}
