#include "ModulePhysics.h"
#include "ModulePhysics.h"
#include "ModulePhysics.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ComponentCollider.h"
#include "ComponentCharacterController.h"
#include "GameObject.h"

#include "ModuleTimeManager.h"
#include "ModuleScripting.h"

#include "PhysX_3.4/Include/extensions/PxDefaultAllocator.h"
#include "PhysX_3.4/Include/extensions/PxDefaultErrorCallback.h"
#include "PhysX_3.4/Include/pvd/PxPvd.h"
#include "PhysX_3.4/Include/pvd/PxPvdSceneClient.h"
#include "PhysX_3.4/Include/pvd/PxPvdTransport.h"
#include "PhysX_3.4/Include/PxPhysicsAPI.h"
#include "PhysX_3.4/Include/characterkinematic/PxControllerManager.h"
#include "PhysX_3.4/Include/foundation/PxAllocatorCallback.h"

#ifndef _DEBUG
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxFoundationCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CharacterKinematicCHECKED_x86.lib")
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
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CharacterKinematicDEBUG_x86.lib")
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

void ModulePhysics::setupFiltering(physx::PxRigidActor* actor, physx::PxU32 LayerMask, physx::PxU32 filterMask)
{
	physx::PxFilterData filterData;
	filterData.word0 = LayerMask; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;

	const physx::PxU32 numShapes = actor->getNbShapes();
	physx::PxShape** shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
	actor->getShapes(shapes, numShapes);
	for (physx::PxU32 i = 0; i < numShapes; i++)
	{
		physx::PxShape* shape = shapes[i];
		shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);
	}
	free(shapes);
}

physx::PxFilterFlags customFilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}

	return physx::PxFilterFlag::eDEFAULT;
}

bool ModulePhysics::Init(json& config)
{
	layer_list.push_back(Layer{ "Default", LayerMask::LAYER_0 });
	layer_list.push_back(Layer{ "Player", LayerMask::LAYER_1 });
	layer_list.push_back(Layer{ "Enemy", LayerMask::LAYER_2 });
	layer_list.push_back(Layer{ "UI", LayerMask::LAYER_3 });
	layer_list.push_back(Layer{ "Ignore Raycast", LayerMask::LAYER_4 });

	for (int i = 0; i < layer_list.size(); ++i) {
		layer_list.at(i).active_layers.resize(layer_list.size(), true);
		if (i == 0)
			layer_list.at(i).UpdateLayerGroup();
		else
			layer_list.at(i).LayerGroup = layer_list.at(0).LayerGroup;
	}

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
	sceneDesc.bounceThresholdVelocity = 9.8 * 0.2;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = customFilterShader;
	//sceneDesc.flags = physx::PxSceneFlag::eENABLE_KINEMATIC_PAIRS | physx::PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;

	mScene = mPhysics->createScene(sceneDesc);

	// This will enable basic visualization of PhysX objects like - actors collision shapes and their axes.
		//The function PxScene::getRenderBuffer() is used to render any active visualization for scene.
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0);	//Global visualization scale which gets multiplied with the individual scales
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);	//Enable visualization of actor's shape
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);	//Enable visualization of actor's axis

	mMaterial = mPhysics->createMaterial(1.0f, 1.0f, 0.0f);

	mControllerManager = PxCreateControllerManager(*mScene);

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
	//if (App->GetAppState() == AppState::PLAY)
	//	SimulatePhysics(dt);

	if (App->GetAppState() == AppState::PLAY)
	{
		// --- Step physics simulation ---
		physAccumulatedTime += App->time->GetRealTimeDt();

		// --- If enough time has elapsed, update ---
		if (physAccumulatedTime >= physx::fixed_dt)
		{
			physAccumulatedTime -= physx::fixed_dt;

		    FixedUpdate();

			mScene->simulate(physx::fixed_dt);
			mScene->fetchResults(true);
		}
	}

	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::FixedUpdate()
{
	App->scripting->GameUpdate(physx::fixed_dt);
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


void ModulePhysics::addActor(physx::PxRigidActor* actor, LayerMask* LayerGroup) {
	actors.insert(std::pair<physx::PxRigidActor*, LayerMask*>(actor, LayerGroup));
	mScene->addActor(*actor);
}

void ModulePhysics::UpdateActor(physx::PxRigidActor* actor, LayerMask* Layermask) {
	//DeleteActor(actor);
	const physx::PxU32 numShapes = actor->getNbShapes();
	physx::PxShape** shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
	actor->getShapes(shapes, numShapes);
	for (physx::PxU32 i = 0; i < numShapes; i++)
	{
		physx::PxShape* shape = shapes[i];

		physx::PxFilterData filterData;
		filterData = shape->getSimulationFilterData();
		filterData.word0 = (1 << *Layermask);

		shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);
	}
	//addActor(actor, Layermask);
	free(shapes);
}

void ModulePhysics::UpdateActors(LayerMask* updateLayer)
{
	if (actors.size() == 0)
		return;

	for (std::map<physx::PxRigidActor*, LayerMask*>::iterator it = actors.begin(); it != actors.end(); ++it)
	{
		LayerMask layer1 = *(*it).second;
		LayerMask layer2 = *updateLayer;
		if (layer1 == layer2) {
			physx::PxFilterData filterData;

			const physx::PxU32 numShapes = (*it).first->getNbShapes();
			physx::PxShape** shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
			(*it).first->getShapes(shapes, numShapes);
			for (physx::PxU32 i = 0; i < numShapes; i++)
			{
				physx::PxShape* shape = shapes[i];
				filterData = shape->getSimulationFilterData();
				filterData.word1 = layer_list.at(layer2).LayerGroup;

				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				shape->setQueryFilterData(filterData);
				shape->setSimulationFilterData(filterData);
			}
			free(shapes);
			break;
		}
	}
}

void ModulePhysics::DeleteActor(physx::PxRigidActor* actor)
{
	if (actors.size() > 0)
	{
		for (std::map<physx::PxRigidActor*, LayerMask*>::iterator it = actors.begin(); it != actors.end(); ++it)
		{
			if ((*it).first == actor) {
				mScene->removeActor(*actor);

				actors.erase(actor);
				break;
			}

		}
	}
}

void ModulePhysics::DeleteActors(GameObject* go)
{
	if (go == nullptr)
		go = App->scene_manager->GetRootGO();

	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			if ((*it)->GetComponent<ComponentCollider>() != nullptr) {
				ComponentCollider* col = (*it)->GetComponent<ComponentCollider>();
				physx::PxRigidActor* actor = col->GetActor();
				actors.erase(actor);
				col->Delete();
			}

			if ((*it)->GetComponent<ComponentCharacterController>() != nullptr)
				(*it)->GetComponent<ComponentCharacterController>()->Delete();
		}
	}
}
