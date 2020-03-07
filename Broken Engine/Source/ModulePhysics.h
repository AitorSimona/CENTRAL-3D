#ifndef MODULE_PHYSICS_H_
#define MODULE_PHYSICS_H_
#include "Module.h"
//
//#include "PhysX_3.4/Include/PxPhysicsAPI.h"
//#include "PhysX_3.4/Include/pvd/PxPvd.h"
//#include "PhysX_3.4/Include/pvd/PxPvdSceneClient.h"
//#include "PhysX_3.4/Include/pvd/PxPvdTransport.h"

namespace physx
{
	class PxPvd;
	class PxPvdSceneClient;
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
	class PxRigidStatic;
}

BE_BEGIN_NAMESPACE

class BROKEN_API ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(json& config) override;
	update_status Update(float dt) override;

	bool CleanUp() override;

	void PlaneCollider(float posX, float posY, float posZ);
	void BoxCollider(float posX, float posY, float posZ);

	void SimulatePhysics(float dt, float speed = 1.0f);

public:
	
	physx::PxPvd* mPvd = nullptr; 
	physx::PxPvdSceneClient* pvdClient = nullptr;
	physx::PxFoundation* mFoundation = nullptr;
	physx::PxPhysics* mPhysics = nullptr;
	physx::PxScene* mScene = nullptr;
	physx::PxMaterial* mMaterial = nullptr;

	physx::PxRigidStatic* plane = nullptr;
};

BE_END_NAMESPACE
#endif
