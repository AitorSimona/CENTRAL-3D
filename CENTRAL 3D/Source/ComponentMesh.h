#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Math.h"
#include "ResourceMesh.h"


class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* ContainerGO);
	virtual ~ComponentMesh();
	const AABB& GetAABB() const;

	// --- Save & Load ---
	void Save(json& file) const override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::Mesh; };
public:
	ResourceMesh* resource_mesh = nullptr;

};

#endif

