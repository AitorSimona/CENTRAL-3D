#include "ResourceFolder.h"
#include "Application.h"
#include "ModuleFileSystem.h"


ResourceFolder::ResourceFolder(uint UID, std::string source_file) : Resource(Resource::ResourceType::FOLDER, UID, source_file)
{
	extension = ".folder";
	resource_file = App->fs->GetDirectoryFromPath(source_file) + std::to_string(UID) + extension;
}

ResourceFolder::~ResourceFolder()
{
}
