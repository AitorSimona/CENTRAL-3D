#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "JSONLoader.h"
#include "Resource.h"

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		Transform,
		Mesh,
		MeshRenderer,
		Camera,
		Unknown
	};

public:

	Component(GameObject* ContainerGO,Component::ComponentType type);
	virtual ~Component();
	virtual void Enable();
	virtual void Disable();
	virtual void Update() {};

	// --- Getters ---
	ComponentType GetType() const;
	GameObject* GetContainerGameObject() const;
	bool& GetActive();
	uint GetUID();

	bool IsEnabled() const;
	void SetUID(uint uid);

	// --- Save & Load ---
	virtual json Save() const = 0;
	virtual void Load(json& node) = 0;
	virtual void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) {};
	virtual void CreateInspectorNode() = 0;
	virtual void DrawComponent() {};

	std::string name;
	bool openinInspector = true;
protected:
	uint UID = 0;
	bool active = false;
	GameObject* GO = nullptr;
	ComponentType type = ComponentType::Unknown;
};

#endif

