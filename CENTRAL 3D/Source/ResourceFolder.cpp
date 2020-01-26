#include "ResourceFolder.h"

ResourceFolder::ResourceFolder(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::FOLDER, UID, source_file, destination_file)
{
}

ResourceFolder::~ResourceFolder()
{
}
