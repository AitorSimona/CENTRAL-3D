#include "ResourceShaderObject.h"

ResourceShaderObject::ResourceShaderObject() : Resource(Resource::ResourceType::SHADER_OBJECT)
{
}

ResourceShaderObject::ResourceShaderObject(uint UID) : Resource(Resource::ResourceType::SHADER_OBJECT, UID)
{
}

ResourceShaderObject::~ResourceShaderObject()
{
}
