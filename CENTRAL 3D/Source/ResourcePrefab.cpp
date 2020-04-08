#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

ResourcePrefab::ResourcePrefab(uint UID, std::string source_file) : Resource(Resource::ResourceType::PREFAB, UID, source_file)
{
	extension = ".prefab";

	std::string tmp = source_file;
	App->fs->SplitFilePath(tmp.c_str(), nullptr, &tmp);

	resource_file = tmp + extension;

	previewTexID = App->gui->prefabTexID;
}

ResourcePrefab::~ResourcePrefab()
{
}

bool ResourcePrefab::LoadInMemory()
{
	return true;
}

void ResourcePrefab::FreeMemory()
{
}

void ResourcePrefab::CreateInspectorNode()
{
}

void ResourcePrefab::OnOverwrite()
{
	NotifyUsers(ResourceNotificationType::Overwrite);

}

void ResourcePrefab::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
