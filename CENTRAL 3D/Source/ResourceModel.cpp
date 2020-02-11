#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "Importer.h"
#include "ImporterModel.h"

#include "mmgr/mmgr.h"

ResourceModel::ResourceModel(uint UID, std::string source_file) : Resource(Resource::ResourceType::MODEL, UID, source_file)
{
	extension = ".model";
	resource_file = MODELS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceModel::~ResourceModel()
{
	resources.clear();
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
	if(!HasResource(resource))
	resources.push_back(resource);
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
	// --- Free Everything ---
	FreeMemory();

	for (uint i = 0; i < resources.size(); ++i)
	{
		resources[i]->OnDelete();
		delete resources[i];
	}

	// --- Delete lib file ---
	OnDelete();

	// --- Ask for reimport ---
	ImporterModel* IModel = App->resources->GetImporter<ImporterModel>();
	
	ImportModelData MData(GetOriginalFile());
	MData.model_overwrite = this;

	IModel->Import(MData);

	if (instances > 0)
		LoadInMemory();
}

void ResourceModel::OnDelete()
{
	FreeMemory();

	App->fs->Remove(resource_file.c_str());

	for (uint i = 0; i < resources.size(); ++i)
	{
		resources[i]->has_parent = true; // so sons know they have to delete their sons (like mat deleting textures)
		resources[i]->OnDelete();
		delete resources[i];
	}

	resources.clear();

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
