#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include "ResourceMaterial.h"


class ComponentMaterial : public Component
{
public:

	ComponentMaterial(Component::ComponentType type);
	virtual ~ComponentMaterial();

	void FreeTexture();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;


	static inline Component::ComponentType GetType() { return Component::ComponentType::Material; };

public:

	ResourceMaterial * resource_material = nullptr;
};

#endif