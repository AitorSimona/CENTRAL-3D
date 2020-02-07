#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResources.h"

#include "Importer.h"

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
	resources.push_back(resource);
}
