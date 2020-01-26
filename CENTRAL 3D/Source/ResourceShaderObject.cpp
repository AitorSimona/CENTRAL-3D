#include "ResourceShaderObject.h"

ResourceShaderObject::ResourceShaderObject(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::SHADER_OBJECT, UID, source_file, destination_file)
{
}

ResourceShaderObject::~ResourceShaderObject()
{
}
