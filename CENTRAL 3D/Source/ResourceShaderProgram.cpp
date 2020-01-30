#include "ResourceShaderProgram.h"
#include "Application.h"
#include "ModuleGui.h"

ResourceShaderProgram::ResourceShaderProgram(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER, UID, source_file)
{
	// MYTODO: Should go to library folder?
	extension = ".shader";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

	previewTexUID = App->gui->defaultfileTexUID;

}

ResourceShaderProgram::~ResourceShaderProgram()
{
}
