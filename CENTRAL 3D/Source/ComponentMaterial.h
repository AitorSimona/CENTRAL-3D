#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"

struct aiScene;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(Component::ComponentType type);
	virtual ~ComponentMaterial();

	void CreateEmpty();
	void CreateFromScene(const aiScene & scene, const char& file_path);

public:
	uint  TextureID = 0;
};

#endif