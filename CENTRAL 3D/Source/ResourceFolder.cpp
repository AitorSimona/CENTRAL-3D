#include "ResourceFolder.h"

ResourceFolder::ResourceFolder() : Resource(Resource::ResourceType::FOLDER)
{
}

ResourceFolder::ResourceFolder(uint UID) : Resource(Resource::ResourceType::FOLDER, UID)
{
}

ResourceFolder::~ResourceFolder()
{
}
