#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "BrokenCore.h"
#include "JSONLoader.h"
#include "Resource.h"

BE_BEGIN_NAMESPACE
class GameObject;

class BROKEN_API Component {
public:
	enum class ComponentType {
		Transform,
		Mesh,
		MeshRenderer,
		Camera,
		Canvas,
		Image,
		Text,
		Button,
		CheckBox,
		InputText,
		ProgressBar,
		CircularBar,
		Script,
		Bone,
		Animation,
		DynamicRigidBody,
		Collider,
		ParticleEmitter,
		AudioSource,
		AudioListener,
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
	uint GetUID() const { return UID; };
	void SetUID(uint new_UID) { UID = new_UID; };

	virtual void Update() {};

	// --- Save & Load ---
	virtual json Save() const = 0;
	virtual void Load(json& node) = 0;
	virtual void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) {};
	virtual void CreateInspectorNode() = 0;

protected:

	bool active = false;
	bool to_delete = false;

	GameObject* GO = nullptr;
	ComponentType type = ComponentType::Unknown;
	uint UID = 0;
};

BE_END_NAMESPACE
#endif
