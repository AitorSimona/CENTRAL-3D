#include "ResourceShaderProgram.h"
#include "Application.h"
#include "ModuleGui.h"

#include "mmgr/mmgr.h"


ResourceShaderProgram::ResourceShaderProgram(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER, UID, source_file)
{
	// MYTODO: Should go to library folder?
	extension = ".shader";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceShaderProgram::~ResourceShaderProgram()
{
}

bool ResourceShaderProgram::LoadInMemory()
{
	return true;

}

void ResourceShaderProgram::FreeMemory()
{
}
