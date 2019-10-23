#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include <string>

struct aiScene;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(Component::ComponentType type);
	virtual ~ComponentMaterial();

	void FreeTexture();
public:
	// --- Diffuse Texture ---
	uint  TextureID = 0;
	std::string Texture_path;
	uint Texture_width = 0;
	uint Texture_height = 0;
	bool checkers = false;
};

#endif