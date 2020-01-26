#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "OpenGL.h"


#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture(uint UID, std::string source_file) : Resource(Resource::ResourceType::TEXTURE, UID, source_file)
{
	extension = ".dds";
	resource_file = TEXTURES_FOLDER + std::to_string(UID) + extension;

	buffer_id = App->textures->GetDefaultTextureID();
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

void ResourceTexture::LoadInMemory()
{
}

void ResourceTexture::FreeMemory()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}
