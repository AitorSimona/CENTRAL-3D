#ifndef __COMPONENT_CHARACTER_CONTROLLER_H__
#define __COMPONENT_CHARACTER_CONTROLLER_H__

#include "Component.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

BE_BEGIN_NAMESPACE

class ResourceMesh;



class BROKEN_API ComponentCharacterController : public Component, public physx::PxControllerBehaviorCallback
{
public:
	ComponentCharacterController(GameObject* ContainerGO);
	virtual ~ComponentCharacterController();

	void Update() override;

	void Draw();
	void DrawComponent() override;

	void SetVelocity(float velX, float velY, float velZ);

	void Move(float minDist = 0.01f);

	void Delete();

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

	virtual physx::PxControllerBehaviorFlags		getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor);
	virtual physx::PxControllerBehaviorFlags		getBehaviorFlags(const physx::PxController&);
	virtual physx::PxControllerBehaviorFlags		getBehaviorFlags(const physx::PxObstacle&);

public: 
	ResourceMesh* mesh = nullptr;
	physx::PxController* controller = nullptr;
	physx::PxVec3 velocity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	bool hasBeenDeactivated = false;

private:
	physx::PxControllerDesc* desc = nullptr;
	physx::PxCapsuleControllerDesc capsuleDesc;

	float contactOffset = 0.01f;
	float stepOffset = 0.3f;
	float slopeLimit = 45.0f;
	float radius = 0.5f;
	float height = 2.0f;
	bool sliding = true;
	bool gravity = true;

	physx::PxVec3 position = physx::PxVec3(0.0f, 0.0f, 0.0f);
	physx::PxExtendedVec3 initialPosition = physx::PxExtendedVec3(0.0f, 0.0f, 0.0f);
	physx::PxVec3 vel = physx::PxVec3(0);

	bool creation = false;
	bool draw = true;
};

BE_END_NAMESPACE

#endif __COMPONENT_CHARACTER_CONTROLLER_H__
