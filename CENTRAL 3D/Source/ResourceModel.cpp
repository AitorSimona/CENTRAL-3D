#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"

#include "mmgr/mmgr.h"

ResourceModel::ResourceModel(uint UID, std::string source_file) : Resource(Resource::ResourceType::MODEL, UID, source_file)
{
	extension = ".model";
	resource_file = MODELS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexUID;

}

ResourceModel::~ResourceModel()
{
}

void ResourceModel::LoadInMemory()
{
}

void ResourceModel::FreeMemory()
{
}
