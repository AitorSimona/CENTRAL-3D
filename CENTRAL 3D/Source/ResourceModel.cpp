#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"


ResourceModel::ResourceModel(uint UID, std::string source_file) : Resource(Resource::ResourceType::MODEL, UID, source_file)
{
	extension = ".model";
	resource_file = MODELS_FOLDER + std::to_string(UID) + extension;

	previewTexUID = App->gui->defaultfileTexUID;

}

ResourceModel::~ResourceModel()
{
}
