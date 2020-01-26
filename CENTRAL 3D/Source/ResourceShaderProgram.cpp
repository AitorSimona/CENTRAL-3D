#include "ResourceShaderProgram.h"

ResourceShaderProgram::ResourceShaderProgram(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::SHADER, UID, source_file, destination_file)
{
}

ResourceShaderProgram::~ResourceShaderProgram()
{
}
