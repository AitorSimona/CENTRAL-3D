#include "ResourceScript.h"

ResourceScript::ResourceScript(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCRIPT, UID, source_file)
{
	extension = ".lua";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

}

ResourceScript::~ResourceScript()
{
}

bool ResourceScript::LoadInMemory()
{
	return false;
}

void ResourceScript::OnOverwrite()
{
}

void ResourceScript::OnDelete()
{
}

void ResourceScript::FreeMemory()
{
}
