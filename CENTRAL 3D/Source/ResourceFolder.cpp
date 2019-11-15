#include "ResourceFolder.h"
#include "mmgr/mmgr.h"

ResourceFolder::ResourceFolder() : Resource(Resource::ResourceType::FOLDER)
{
}

ResourceFolder::~ResourceFolder()
{
}

std::vector<Resource*> ResourceFolder::GetChilds() const
{
	return childs;
}
