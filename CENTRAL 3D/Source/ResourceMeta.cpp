#include "ResourceMeta.h"
#include "Application.h"
#include "ModuleFileSystem.h"


ResourceMeta::ResourceMeta(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::META, UID, source_file, destination_file)
{
	Date = App->fs->GetLastModificationTime(source_file.c_str());
}

ResourceMeta::~ResourceMeta()
{
}



