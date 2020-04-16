#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "Importer.h"
#include "ImporterModel.h"
#include "ImporterMaterial.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceModel::ResourceModel(uint UID, const char* source_file) : Resource(Resource::ResourceType::MODEL, UID, source_file) {
	extension = ".model";
	resource_file = MODELS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->prefabTexID;

}

ResourceModel::~ResourceModel() 
{
	resources.clear();
	glDeleteTextures(1, (GLuint*)&previewTexID);
}

bool ResourceModel::LoadInMemory() 
{
	// MYTODO: if we previously saved all resources in the .model, load them here (ask resource manager)

	// --- Get all child resources, effectively loading them in memory as instances go beyond 0 ---

	//for (uint i = 0; i < resources.size(); ++i)
	//{
	//	App->resources->GetResource(resources[i]->GetUID());
	//}


	return true;
}

void ResourceModel::FreeMemory()
{

}

void ResourceModel::AddResource(Resource* resource) 
{
	if (!HasResource(resource))
	{
		resource->has_parent = true;
		resources.push_back(resource);
	}
}

void ResourceModel::RemoveResource(Resource* resource) 
{
	for (std::vector<Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if ((*it)->GetUID() == resource->GetUID()) 
		{
			resources.erase(it);
			break;
		}
	}
}

std::vector<Resource*>* ResourceModel::GetResources() 
{
	return &resources;
}

bool ResourceModel::HasResource(Resource* resource) 
{
	bool found = false;

	for (std::vector<Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it) 
	{
		if ((*it)->GetUID() == resource->GetUID())
		{
			found = true;
			break;
		}
	}

	return found;
}

void ResourceModel::OnOverwrite() 
{
	NotifyUsers(ResourceNotificationType::Overwrite);

	// --- Keep a copy of current resources pointers ---
	std::vector<Resource*> old_resources = resources;
	resources.clear();

	// --- Ask for reimport, filling resources list with the new ones ---
	ImporterModel* IModel = App->resources->GetImporter<ImporterModel>();

	ImportModelData MData(GetOriginalFile());
	MData.model_overwrite = this;

	IModel->Import(MData);

	// --- Overwrite childs ---
	for (uint i = 0; i < old_resources.size(); ++i) 
	{
		App->resources->RemoveResourceFromFolder(old_resources[i]);
		App->resources->ONResourceDestroyed(old_resources[i]);
		old_resources[i]->OnOverwrite();
		old_resources[i]->ClearUsers();
		old_resources[i]->OnDelete();

		delete old_resources[i];
	}

	old_resources.clear();


	if (instances > 0)
		LoadInMemory();
}

void ResourceModel::OnDelete() 
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();

	App->fs->Remove(resource_file.c_str());
	App->fs->Remove(previewTexPath.c_str());

	for (uint i = 0; i < resources.size(); ++i) 
	{
		resources[i]->OnDelete();
		delete resources[i];
	}

	resources.clear();

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
