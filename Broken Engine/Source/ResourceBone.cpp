#include "ResourceBone.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "mmgr/mmgr.h"


ResourceBone::ResourceBone(uint UID, std::string source_file) : Resource(Resource::ResourceType::BONE, UID, source_file)
{
	extension = ".bone";
	resource_file = BONES_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceBone::~ResourceBone()
{
}

bool ResourceBone::LoadInMemory()
{
	return true;
}

void ResourceBone::FreeMemory()
{

}

void ResourceBone::OnOverwrite()
{
	// Since mesh is not a standalone resource (which means it is always owned by a model) the model is in charge
	// of overwriting it (see ResourceModel OnOverwrite for details)
	NotifyUsers(ResourceNotificationType::Overwrite);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());
}

void ResourceBone::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceBone::Repath()
{

}