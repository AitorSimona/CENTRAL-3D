#ifndef MODULE_PHYSICS_H_
#define MODULE_PHYSICS_H_
#include "Module.h"

namespace physx
{
	class PxPvd;
	class PxPvdSceneClient;
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
	class PxRigidStatic;

	const float fixed_dt = (1.0f/60.0f);
}

BE_BEGIN_NAMESPACE
class GameObject;

class BROKEN_API ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(json& config) override;
	update_status Update(float dt) override;
	void FixedUpdate();

	bool CleanUp() override;

	void PlaneCollider(float posX, float posY, float posZ);
	void BoxCollider(float posX, float posY, float posZ);

	void SimulatePhysics(float dt, float speed = 1.0f);

	void DeleteActors(GameObject* go = nullptr);

public:
	
	physx::PxPvd* mPvd = nullptr; 
	physx::PxPvdSceneClient* pvdClient = nullptr;
	physx::PxFoundation* mFoundation = nullptr;
	physx::PxPhysics* mPhysics = nullptr;
	physx::PxScene* mScene = nullptr;
	physx::PxMaterial* mMaterial = nullptr;

	physx::PxRigidStatic* plane = nullptr;

private:

	float physAccumulatedTime = 0.0f;
};

BE_END_NAMESPACE
#endif
