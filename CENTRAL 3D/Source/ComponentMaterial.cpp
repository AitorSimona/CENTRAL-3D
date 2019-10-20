#include "ComponentMaterial.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(Component::ComponentType type) : Component(type)
{
	// MYTODO: We are not setting any container GO, should point to future root?
}

ComponentMaterial::~ComponentMaterial()
{	
	glDeleteTextures(1, (GLuint*)&TextureID);
}

void ComponentMaterial::FreeTexture()
{
	glDeleteTextures(1, (GLuint*)&TextureID);
}



