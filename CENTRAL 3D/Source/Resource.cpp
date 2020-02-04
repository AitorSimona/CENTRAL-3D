#include "Resource.h"
#include "Application.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"


// MYTODO: TO BE DELETED ONCE SHADERS REWORK IS DONE 
Resource::Resource(ResourceType type)
{
	this->type = type;
}

Resource::Resource(ResourceType type, uint UID, std::string source_file)
{
	this->type = type;
	this->UID = UID;
	original_file = source_file;
	name = source_file;
}

Resource::~Resource()
{
	App->resources->ONResourceDestroyed(this);
}

uint Resource::GetUID() const
{
	return UID;
}

void Resource::SetUID(uint UID)
{ 
	this->UID = UID;
}

Resource::ResourceType Resource::GetType() const
{
	return type;
}

const char * Resource::GetOriginalFile() const
{
	return original_file.c_str();
}

const char * Resource::GetResourceFile() const
{
	return resource_file.c_str();
}

const char* Resource::GetExtension() const
{
	return extension.c_str();
}

const char * Resource::GetName() const
{
	return name.c_str();
}

const uint Resource::GetPreviewTexID() const
{
	return previewTexID;
}

void Resource::SetName(const char * name)
{
	this->name = name;
}

void Resource::SetOriginalFilename(const char * filename)
{
	std::string new_name = filename;
	this->original_file = new_name;
}
