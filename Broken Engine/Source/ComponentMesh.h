#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Math.h"
#include "Component.h"
#include "ResourceMesh.h"

namespace BrokenEngine {
	class ComponentMesh : public Component {
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
	};
}
#endif

