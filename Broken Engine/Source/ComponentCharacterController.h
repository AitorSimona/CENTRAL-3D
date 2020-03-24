#ifndef __COMPONENT_CHARACTER_CONTROLLER_H__
#define __COMPONENT_CHARACTER_CONTROLLER_H__

#include "Component.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

BE_BEGIN_NAMESPACE

class ResourceMesh;

class BROKEN_API ComponentCharacterController : public Component
{
public:
	ComponentCharacterController(GameObject* ContainerGO);
	virtual ~ComponentCharacterController();

	void Update() override;

	void Draw();

	static inline Component::ComponentType GetType() { return Component::ComponentType::CharacterController; };

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	// --- Setters ---
	void SetContactOffset(float offset);
	void SetStepOffset(float offset);
	void SetSlopeLimit(float limit);
	void SetRadius(float radius);
	void SetHeight(float height);

public: 
	ResourceMesh* mesh = nullptr;

private:
	physx::PxControllerDesc* desc = nullptr;
	physx::PxCapsuleControllerDesc capsuleDesc;
	physx::PxController* controller = nullptr;

	float contactOffset = 0.05f;
	float stepOffset = 0.01f;
	float slopeLimit = 0.5f;
	float radius = 0.5f;
	float height = 1.0f;
};

BE_END_NAMESPACE

#endif __COMPONENT_CHARACTER_CONTROLLER_H__
