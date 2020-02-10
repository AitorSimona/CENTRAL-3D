#include "ResourceShaderObject.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"


#include "mmgr/mmgr.h"

ResourceShaderObject::ResourceShaderObject(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER_OBJECT, UID, source_file)
{
	// MYTODO: Should be .vert, .frag ... should go to library folder? ---
	extension = ".shobj";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceShaderObject::~ResourceShaderObject()
{
}

bool ResourceShaderObject::LoadInMemory()
{
	return true;

}

void ResourceShaderObject::FreeMemory()
{
}

void ResourceShaderObject::OnOverwrite()
{
}

void ResourceShaderObject::OnDelete()
{
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
