#include "ModulePhysics.h"


#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#ifndef _DEBUG
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxFoundationCHECKED_x86.lib")
/*
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Extensions.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PxFoundation_x86.lib")
*/
#else
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxFoundationDEBUG_x86.lib")
#endif // _DEBUG


ModulePhysics::ModulePhysics(bool start_enabled)
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init(json config)
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		CONSOLE_LOG("PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;

	mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation,
		PxTolerancesScale(), recordMemoryAllocations);
	if (!mPhysics) {
		CONSOLE_LOG("PxCreateBasePhysics failed!");
		return false;
	}

	PxRegisterParticles(*mPhysics);

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	mScene = mPhysics->createScene(sceneDesc);

	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	BoxCollider(0, 0, 0);

	return true;
}

update_status ModulePhysics::Update(float dt)
{
	SimulatePhysics(dt);

	return update_status::UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	/*mPhysics->release();
	mFoundation->release();

	mPhysics = nullptr;
	mFoundation = nullptr;*/
	mScene->release();
	mScene = nullptr;
	return false;
}

// Creating static plane
void ModulePhysics::PlaneCollider(float posX, float posY, float posZ)
{
	PxTransform position = PxTransform(PxVec3(posX, posY, posZ), PxQuat(PxHalfPi, PxVec3(0.0f, 0, 0.0f)));
	PxRigidStatic* plane = mPhysics->createRigidStatic(position);
	plane->createShape(PxPlaneGeometry(), *mMaterial);
	mScene->addActor(*plane);
}

void ModulePhysics::BoxCollider(float posX, float posY, float posZ)
{
	PxRigidDynamic* box;
	PxTransform position(PxVec3(posX, posY, posZ));
	PxBoxGeometry geometry(PxVec3(0.5f, 0.5f, 0.5f));

	box = PxCreateDynamic(*mPhysics, position, geometry, *mMaterial, 1.0f);
	mScene->addActor(*box);

	CONSOLE_LOG("box created");
}

void ModulePhysics::SimulatePhysics(float dt, float speed)
{
	mScene->simulate(dt * speed);
	mScene->fetchResults(true);
}
