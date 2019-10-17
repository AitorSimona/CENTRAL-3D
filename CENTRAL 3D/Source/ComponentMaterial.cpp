#include "ComponentMaterial.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(Component::ComponentType type) : Component(type)
{
	// MYTODO: We are not setting any container GO, should point to future root?
}

ComponentMaterial::~ComponentMaterial()
{	
	glDeleteBuffers(1, (GLuint*)&TextureID);
}
