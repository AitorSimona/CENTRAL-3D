#include "ResourceShaderProgram.h"

ResourceShaderProgram::ResourceShaderProgram() : Resource(Resource::ResourceType::SHADER)
{
}

ResourceShaderProgram::ResourceShaderProgram(uint UID) : Resource(Resource::ResourceType::SHADER, UID)
{
}

ResourceShaderProgram::~ResourceShaderProgram()
{
}
