#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"

namespace math
{
	class AABB;
}

BE_BEGIN_NAMESPACE
class ComponentBone;
class ResourceMesh;


class BROKEN_API ComponentMesh : public Component {
public:

	ComponentMesh(GameObject* ContainerGO);
	virtual ~ComponentMesh();

	void Update() override;

	const math::AABB& GetAABB() const;

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode() override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::Mesh; };

	// -- For skinning
	void AddBone(ComponentBone* bone);
	void UpdateDefMesh();

public:
	ResourceMesh* resource_mesh = nullptr;
	ResourceMesh* deformable_mesh = nullptr;

	std::vector<ComponentBone*> bones;
};
BE_END_NAMESPACE
#endif

