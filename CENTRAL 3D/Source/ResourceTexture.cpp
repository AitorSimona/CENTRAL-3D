#include "ResourceTexture.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceTexture::ResourceTexture() : Resource(Resource::ResourceType::TEXTURE)
{
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

void ResourceTexture::LoadOnMemory()
{
}

void ResourceTexture::FreeMemory()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}
