#include "ComponentMaterial.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(Component::ComponentType type) : Component(type)
{
	// MYTODO: We are not setting any container GO, should point to future root?
}

ComponentMaterial::~ComponentMaterial()
{	
	resource_material->resource_diffuse->instances--;
}

void ComponentMaterial::FreeTexture()
{
	resource_material->resource_diffuse->FreeMemory();
}



