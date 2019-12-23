#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "OpenGL.h"


#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture() : Resource(Resource::ResourceType::TEXTURE)
{
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
