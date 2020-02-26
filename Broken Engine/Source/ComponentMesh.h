#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Math.h"
#include "ResourceMesh.h"

class ComponentBone;

class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* ContainerGO);
	virtual ~ComponentMesh();
	const AABB& GetAABB() const;

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode() override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::Mesh; };

public:
	ResourceMesh* resource_mesh = nullptr;
	ResourceMesh* resource_def_mesh = nullptr;

private:
	void AddBone(ComponentBone* bone);
	void UpdateDefMesh();

private:
	std::vector<ComponentBone*> bones;
};

#endif

