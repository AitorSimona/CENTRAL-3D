#include "ResourceMeta.h"
#include "Application.h"
#include "ModuleGui.h"

#include "ModuleFileSystem.h"

#include "mmgr/mmgr.h"



ResourceMeta::ResourceMeta(uint UID, std::string source_file) : Resource(Resource::ResourceType::META, UID, source_file)
{
	extension = ".meta";
	resource_file = source_file + extension;

	Date = App->fs->GetLastModificationTime(source_file.c_str());

	previewTexID = App->gui->defaultfileTexUID;

}

ResourceMeta::~ResourceMeta()
{
}

void ResourceMeta::LoadInMemory()
{
}

void ResourceMeta::FreeMemory()
{
}



