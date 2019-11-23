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
	jsonmeta["DATE"] = std::to_string(App->fs->GetLastModificationTime(filename));
	jsondata = App->GetJLoader()->Serialize(jsonmeta);
	meta_buffer = (char*)jsondata.data();


	meta_path = filename;
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

	if (App->fs->Exists(path.data()))
	{
		json file = App->GetJLoader()->Load(path.data());
		std::string uid = file["UID"];
		UID = std::stoi(uid);
	}

	return UID;
}

uint ModuleResources::GetModDateFromMeta(const char * file)
{
	std::string path = file;
	path.append(".meta");
	uint DATE = 0;

	if (App->fs->Exists(path.data()))
	{
		json file = App->GetJLoader()->Load(path.data());
		std::string date = file["DATE"];
		DATE = std::stoi(date);
	}

	return DATE;
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



