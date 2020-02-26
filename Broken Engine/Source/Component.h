#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "BrokenCore.h"
#include "JSONLoader.h"
#include "Resource.h"

namespace BrokenEngine {
	class GameObject;

	class BROKEN_API Component {
	public:
		enum class ComponentType {
			Transform,
			Mesh,
			MeshRenderer,
			Camera,
			Script,
			Unknown
		};

	public:

		Component(GameObject* ContainerGO, Component::ComponentType type);
		virtual ~Component();
		virtual void Enable();
		virtual void Disable();

		// --- Getters ---
		ComponentType GetType() const;
		GameObject* GetContainerGameObject() const;
		bool& GetActive();
		bool IsEnabled() const;

		// --- Save & Load ---
		virtual json Save() const = 0;
		virtual void Load(json& node) = 0;
		virtual void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) {};
		virtual void CreateInspectorNode() = 0;

	protected:
		bool active = false;
		GameObject* GO = nullptr;

		ComponentType type = ComponentType::Unknown;
	};
}
#endif

