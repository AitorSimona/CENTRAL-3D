#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include "ResourceMaterial.h"

class GameObject;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* ContainerGO);
	virtual ~ComponentMaterial();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::Material; };

public:

	ResourceMaterial * resource_material = nullptr;
};

#endif