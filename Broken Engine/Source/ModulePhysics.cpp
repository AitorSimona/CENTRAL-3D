#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ComponentCollider.h"
#include "GameObject.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"
#include "PhysX_3.4/Include/pvd/PxPvd.h"
#include "PhysX_3.4/Include/pvd/PxPvdSceneClient.h"
#include "PhysX_3.4/Include/pvd/PxPvdTransport.h"
#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#ifndef _DEBUG
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxFoundationCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxPvdSDKCHECKED_x86.lib")
/*
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Extensions.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PxFoundation_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxPvdSDK_x86.lib")
*/
#else
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxPvdSDKDEBUG_x86.lib")
#endif // _DEBUG

#include "mmgr/mmgr.h"

using namespace Broken;

ModulePhysics::ModulePhysics(bool start_enabled)
{
	name = "Physics";
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init(json& config)
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		ENGINE_CONSOLE_LOG("PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;

	//Setup Connection-----------------------------------------------------------------------
	physx::PxPvdTransport* mTransport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10000);

	if (mTransport == NULL)
		return false;

	physx::PxPvdInstrumentationFlags mPvdFlags = physx::PxPvdInstrumentationFlag::eALL;
	mPvd = physx::PxCreatePvd(*mFoundation);
	mPvd->connect(*mTransport, mPvdFlags);
	//---------------------------------------------------------------------------------------

	mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation,
		physx::PxTolerancesScale(), recordMemoryAllocations,mPvd);
	if (!mPhysics) {
		ENGINE_CONSOLE_LOG("PxCreateBasePhysics failed!");
		return false;
	}

	PxRegisterParticles(*mPhysics);

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	mScene = mPhysics->createScene(sceneDesc);

	// This will enable basic visualization of PhysX objects like - actors collision shapes and their axes.
		//The function PxScene::getRenderBuffer() is used to render any active visualization for scene.
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0);	//Global visualization scale which gets multiplied with the individual scales
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);	//Enable visualization of actor's shape
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);	//Enable visualization of actor's axis

	mMaterial = mPhysics->createMaterial(1.0f, 1.0f, 0.0f);

	//Setup Configuration-----------------------------------------------------------------------
	pvdClient = mScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	//-------------------------------------
	//BoxCollider(0, 10, 0);
	PlaneCollider(0, 0, 0);
	
	return true;
}

update_status ModulePhysics::Update(float dt)
{
	if (App->GetAppState() == AppState::PLAY)
		SimulatePhysics(dt);

	return update_status::UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	mScene->release();
	mPhysics->release();
	mPvd->release();
	mFoundation->release();

	mPhysics = nullptr;
	mFoundation = nullptr;
	mScene = nullptr;
	mPvd = nullptr;

	return true;
}

// Creating static plane
void ModulePhysics::PlaneCollider(float posX, float posY, float posZ)
{
	physx::PxTransform position = physx::PxTransform(physx::PxVec3(posX, posY, posZ), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0, 0.0f)));
	plane = mPhysics->createRigidStatic(position);
	plane = PxCreatePlane(*mPhysics, physx::PxPlane(physx::PxVec3(0, 1, 0), 0), *mMaterial);
	mScene->addActor(*plane);
}

void ModulePhysics::BoxCollider(float posX, float posY, float posZ)
{
	physx::PxRigidDynamic* box;
	physx::PxTransform position(physx::PxVec3(posX, posY, posZ));
	physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));

	box = PxCreateDynamic(*mPhysics, position, geometry, *mMaterial, 1.0f);
	mScene->addActor(*box);

	ENGINE_CONSOLE_LOG("box created");
}

void ModulePhysics::SimulatePhysics(float dt, float speed)
{
	mScene->simulate(dt * speed);
	mScene->fetchResults(true);
}

void ModulePhysics::DeleteActors(GameObject* go)
{
	if (go == nullptr)
		go = App->scene_manager->GetRootGO();

	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			if ((*it)->GetComponent<ComponentCollider>() != nullptr)
				(*it)->GetComponent<ComponentCollider>()->Delete();
		}
	}
}

