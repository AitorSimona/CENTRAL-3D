#include "ResourceMeta.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"

#include "ImporterMeta.h"

#include "ModuleFileSystem.h"

#include "mmgr/mmgr.h"


using namespace Broken;
ResourceMeta::ResourceMeta(uint UID, const char* source_file) : Resource(Resource::ResourceType::META, UID, source_file) 
{
	extension = ".meta";
	resource_file = source_file + extension;

	Date = App->fs->GetLastModificationTime(source_file);

	previewTexID = App->gui->defaultfileTexID;

}

ResourceMeta::~ResourceMeta() 
{
}

bool ResourceMeta::LoadInMemory() 
{
	return true;

}

void ResourceMeta::FreeMemory() 
{
}

void ResourceMeta::OnOverwrite() 
{
	NotifyUsers(ResourceNotificationType::Overwrite);

	// If a file is overwritten, its meta is notified so it can update the last modification time of its assigned resource

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	Resource* resource = App->resources->GetResource(UID, false);

	if (resource)
		Date = App->fs->GetLastModificationTime(resource->GetOriginalFile());

	IMeta->Save(this);

}

void ResourceMeta::OnDelete() 
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceMeta::Repath()
{
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	IMeta->Save(this);
}




