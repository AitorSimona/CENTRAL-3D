#include "ModuleResources.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

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
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = resources.erase(it);
	}

	return true;
}

void ModuleResources::CreateMetaFromUID(uint UID,const char* filename)
{
	ResourceMeta meta;

	json jsonmeta;
	std::string jsondata;
	std::string meta_path;
	char* meta_buffer = nullptr;

	// --- Create Meta ---
	jsonmeta["UID"] = std::to_string(UID);
	jsondata = App->GetJLoader()->Serialize(jsonmeta);
	meta_buffer = (char*)jsondata.data();

	meta_path = ASSETS_FOLDER;
	meta_path.append(filename);
	meta_path.append(".meta");

	LoadedResources[UID] = meta;
	App->fs->Save(meta_path.data(), meta_buffer, jsondata.length());
}

bool ModuleResources::IsFileImported(const char * file)
{
	bool ret = false;

	std::string path = file;

	path.append(".meta");

	ret = App->fs->Exists(path.data());

	return ret;
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
			ret->SetOriginalFilename(it->second.original_file.data());
			ret->SetName(it->second.resource_name.data());
			LOG("Loaded Resource: %s", ret->GetName());
		}
	}

	return ret;
}

Resource * ModuleResources::GetResource(const char * original_file)
{
	// --- If resource is loaded into memory, return pointer to it, else load it ---

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		std::string tmp = it->second->GetOriginalFile();
		if (tmp.compare(original_file) == 0)
			return it->second;
	}

	return nullptr;
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

uint ModuleResources::GetUIDFromMeta(const char * file)
{
	std::string path = file;
	path.append(".meta");
	uint UID = 0;
	path = path.substr(1, path.size());

	if (App->fs->Exists(path.data()))
	{
		json file = App->GetJLoader()->Load(path.data());
		std::string uid = file["UID"];
		UID = std::stoi(uid);
	}

	return UID;
}

Resource * ModuleResources::CreateResource(Resource::ResourceType type)
{
	Resource* resource = nullptr;

	switch (type)
	{
	case Resource::ResourceType::MESH:
		resource = (Resource*)new ResourceMesh;
		break;

	case Resource::ResourceType::TEXTURE:
		resource = (Resource*)new ResourceTexture;
		break;

	case Resource::ResourceType::MATERIAL:
		resource = (Resource*)new ResourceMaterial;
		break;
	}

	if (resource)
		resources[resource->GetUID()] = resource;

	return resource;
}

//void ModuleResources::AddResource(Resource * resource)
//{
//	if(resource)
//	resources[resource->GetUID()] = resource;
//	
//}

void ModuleResources::LoadResource(Resource * resource)
{
	resources[resource->GetUID()] = resource;
	resource->LoadOnMemory();
}

uint ModuleResources::DeleteResource(uint UID)
{
	Resource::ResourceType type = LoadedResources[UID].type;
	uint instances = 0;

	// --- If resource exists in Library destroy it ---
	if (resources[UID])
	{
		instances = resources[UID]->instances;
		UnloadResource(UID);
		delete resources[UID];
		resources.erase(UID);
	}

	return uint();
}

void ModuleResources::UnloadResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end())
	{
		it->second->FreeMemory();
		Resource* resource = it->second;
		resources.erase(it);
		delete resource;
	}
}

