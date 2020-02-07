#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleTextures.h"
#include "OpenGL.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture(uint UID, std::string source_file) : Resource(Resource::ResourceType::TEXTURE, UID, source_file)
{
	extension = ".dds";
	resource_file = TEXTURES_FOLDER + std::to_string(UID) + extension;
	buffer_id = App->textures->GetDefaultTextureID();
	previewTexID = App->gui->defaultfileTexID;

	if(App->resources->IsFileImported(source_file.c_str()))
		SetTextureID(App->textures->CreateTextureFromFile(source_file.c_str(), Texture_width, Texture_height, -1));
	else if (source_file != "DefaultTexture")
		SetTextureID(App->textures->CreateTextureFromFile(App->resources->DuplicateIntoAssetsFolder(source_file.c_str()).c_str(), Texture_width, Texture_height, GetUID()));
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

bool ResourceTexture::LoadInMemory()
{
	return true;
}

void ResourceTexture::FreeMemory()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

void ResourceTexture::SetTextureID(uint ID)
{
	buffer_id = previewTexID = ID;
}

uint ResourceTexture::GetTexID()
{
	return buffer_id;
}
