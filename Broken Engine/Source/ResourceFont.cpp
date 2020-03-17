#include "ResourceFont.h"
#include "Application.h"
#include "ModuleGui.h"

#include "mmgr/mmgr.h"

using namespace Broken;



ResourceFont::ResourceFont(uint UID, const char* source_file) : Resource(Resource::ResourceType::FONT, UID, source_file)
{
	extension = ".ttf";
	resource_file = FONTS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;
}

ResourceFont::~ResourceFont()
{
}

bool ResourceFont::LoadInMemory()
{
	return false;
}

void ResourceFont::FreeMemory()
{
	// delete vbo vao texture
	
}

void ResourceFont::OnOverwrite()
{
}

void ResourceFont::OnDelete()
{
}

