#include "ResourceFolder.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"


ResourceFolder::ResourceFolder(uint UID, std::string source_file) : Resource(Resource::ResourceType::FOLDER, UID, source_file)
{
	extension = ".folder";
	resource_file = source_file;
	previewTexUID = App->gui->folderTexUID;

	std::string tmp = source_file;
	tmp.pop_back();
	App->fs->SplitFilePath(tmp.c_str(), nullptr, &tmp);
	tmp.append("/");
	name = tmp;
}

ResourceFolder::~ResourceFolder()
{
}

void ResourceFolder::AddResource(Resource* resource)
{
	resources.push_back(resource);
}

void ResourceFolder::SetParent(ResourceFolder* parent)
{
	if (parent)
	{
		this->parent = parent;
		parent->childs.push_back(this);
	}
}

const std::vector<Resource*>& ResourceFolder::GetResources()
{
	return resources;
}

const std::vector<ResourceFolder*>& ResourceFolder::GetChilds()
{
	return childs;
}

ResourceFolder* ResourceFolder::GetParent() const
{
	return parent;
}
