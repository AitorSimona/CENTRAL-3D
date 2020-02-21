#ifndef MODULE_PHYSICS_H_
#define MODULE_PHYSICS_H_
#include "Module.h"
#include "Globals.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

using namespace physx;

class ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(json config) override;
	update_status Update(float dt) override;

	bool CleanUp() override;

	void PlaneCollider(float posX, float posY, float posZ);
	void BoxCollider(float posX, float posY, float posZ);

	void SimulatePhysics(float dt, float speed = 1.0f);

public:

	PxFoundation* mFoundation = nullptr;
	PxPhysics* mPhysics = nullptr;
	PxScene* mScene = nullptr;
	PxMaterial* mMaterial = nullptr;
};
#endif
