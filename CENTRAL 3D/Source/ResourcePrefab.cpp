#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "PanelProject.h"

#include "mmgr/mmgr.h"

ResourcePrefab::ResourcePrefab(uint UID, std::string source_file) : Resource(Resource::ResourceType::PREFAB, UID, source_file)
{
	extension = ".prefab";

	resource_file = source_file;
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
