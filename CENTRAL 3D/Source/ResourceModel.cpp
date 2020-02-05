#include "ResourceModel.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

ResourceModel::ResourceModel(uint UID, std::string source_file) : Resource(Resource::ResourceType::MODEL, UID, source_file)
{
	extension = ".model";
	resource_file = MODELS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexUID;

}

ResourceModel::~ResourceModel()
{
	resources.clear();
}

bool ResourceModel::LoadInMemory()
{
	// MYTODO: if we previously saved all resources in the .model, load them here (ask resource manager)
	// --- Load model file ---
	json file = App->GetJLoader()->Load(resource_file.c_str());

	if (!file.is_null())
	{
		// --- Iterate main nodes ---
		for (json::iterator it = file.begin(); it != file.end(); ++it)
		{
			// --- Iterate components ---
			json components = file[it.key()]["Components"];

			for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
			{
				// --- Iterate and load resources ---
				json _resources = components[it2.key()]["Resources"];

				if (!_resources.is_null())
				{
					for (json::iterator it3 = _resources.begin(); it3 != _resources.end(); ++it3)
					{
						std::string value = _resources[it3.key()];
						Resource* resource = App->resources->ImportAssets(value.c_str());
						resources.push_back(resource);
					}
				}

			}

		}
	}


	return true;
}

void ResourceModel::FreeMemory()
{
}
