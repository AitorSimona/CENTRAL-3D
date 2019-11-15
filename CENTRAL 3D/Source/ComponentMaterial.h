#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include <string>
#include "ResourceMaterial.h"

struct aiScene;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(Component::ComponentType type);
	virtual ~ComponentMaterial();

	void FreeTexture();
public:

	ResourceMaterial * resource_material = nullptr;
};

#endif