#include "ModulePhysics.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ComponentCollider.h"
#include "ComponentCharacterController.h"
#include "GameObject.h"
#include "PhysxSimulationEvents.h"

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
#include "PhysX_3.4/Include/PxQueryReport.h"
#include "PhysX_3.4/Include/PxVolumeCache.h"
#include "PhysX_3.4/Include/PxQueryFiltering.h"

#ifndef _DEBUG
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxFoundationCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/PhysX3CharacterKinematicCHECKED_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Checked/SceneQueryCHECKED.lib")
/*
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3Extensions.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PxFoundation_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PxPvdSDK_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/PhysX3CharacterKinematic_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Release/SceneQuery.lib")
*/
#else
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysX_3.4/lib/Debug/SceneQueryDEBUG.lib")
#endif // _DEBUG

//#include "mmgr/mmgr.h"

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
	if (actor) {
		physx::PxFilterData filterData;
		filterData.word0 = LayerMask; // word0 = own ID
		filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;

		const physx::PxU32 numShapes = actor->getNbShapes();
		physx::PxShape** shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
		actor->getShapes(shapes, numShapes);
		for (physx::PxU32 i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = shapes[i];

			shape->setSimulationFilterData(filterData);

			shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			shape->setQueryFilterData(filterData);
		}
	}
}

physx::PxFilterFlags customFilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// Let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
	}
	else {

		if ((filterData0.word0 != 0 || filterData1.word0 != 0) &&
			!(filterData0.word0 & filterData1.word1 || filterData1.word0 & filterData0.word1))
			return physx::PxFilterFlag::eSUPPRESS;
		else
		{
			// Generate contacts for all that were not filtered above
			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
			pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		}
	}

	return physx::PxFilterFlag::eDEFAULT;
}

bool ModulePhysics::Init(json& config)
{
	LoadStatus(config);

	if (!loaded) {
		layer_list.push_back(Layer{ "Default", LayerMask::LAYER_0, true });
		layer_list.push_back(Layer{ "Player", LayerMask::LAYER_1, true });
		layer_list.push_back(Layer{ "Enemy", LayerMask::LAYER_2, true });
		layer_list.push_back(Layer{ "UI", LayerMask::LAYER_3, true });
		layer_list.push_back(Layer{ "Ignore Raycast", LayerMask::LAYER_4, true });

		int count = layer_list.size();
		for (int i = 0; i < 10 - count; ++i) {
			layer_list.push_back(Layer{"", LayerMask::LAYER_NONE, false });
		}

		for (int i = 0; i < layer_list.size(); ++i) {
			layer_list.at(i).active_layers.resize(layer_list.size(), true);
			if (i == 0)
				layer_list.at(i).UpdateLayerGroup();
			else
				layer_list.at(i).LayerGroup = layer_list.at(0).LayerGroup;
		}
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

	simulationEventsCallback = new PhysxSimulationEvents(this);

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -gravity, 0.0f);
	sceneDesc.bounceThresholdVelocity = gravity * 0.2;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_KINEMATIC_PAIRS | physx::PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;
	sceneDesc.filterShader = customFilterShader;
	sceneDesc.simulationEventCallback = simulationEventsCallback;
	mScene = mPhysics->createScene(sceneDesc);

	// This will enable basic visualization of PhysX objects like - actors collision shapes and their axes.
		//The function PxScene::getRenderBuffer() is used to render any active visualization for scene.
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0);	//Global visualization scale which gets multiplied with the individual scales
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);	//Enable visualization of actor's shape
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);	//Enable visualization of actor's axis

	mMaterial = mPhysics->createMaterial(materialDesc.x, materialDesc.y, materialDesc.z);

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
	//PlaneCollider(0, 0, 0);

	cache = mScene->createVolumeCache(32, 8);

	return true;
}

update_status ModulePhysics::Update(float dt)
{
	//if (App->GetAppState() == AppState::PLAY)
	//	SimulatePhysics(dt);

	if (App->GetAppState() == AppState::PLAY && !App->time->gamePaused)
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
	cache->release();	//195
	mControllerManager->release();//182
	mScene->release(); //172
	mPhysics->release(); //153
	mPvd->release(); //149
	mFoundation->release(); //136

	mControllerManager = nullptr;
	mPhysics = nullptr;
	mFoundation = nullptr;
	mScene = nullptr;
	mPvd = nullptr;

	RELEASE(simulationEventsCallback);

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


void ModulePhysics::addActor(physx::PxRigidActor* actor, GameObject* gameObject) {
	actors.insert(std::pair<physx::PxRigidActor*, GameObject*>(actor, gameObject));
	mScene->addActor(*actor);
}

void ModulePhysics::UpdateActorLayer(physx::PxRigidActor* actor, LayerMask* Layermask) {
	physx::PxShape* shape;
	actor->getShapes(&shape, 1);

	physx::PxFilterData* filterData;
	filterData = &shape->getSimulationFilterData();
	filterData->word0 = (1 << *Layermask);

	shape->setSimulationFilterData(*filterData);

	shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	shape->setQueryFilterData(*filterData);
}

void ModulePhysics::UpdateActorsGroupFilter(LayerMask* updateLayer)
{
	if (actors.size() == 0)
		return;

	for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = actors.begin(); it != actors.end(); ++it)
	{
		LayerMask layer1 = (*it).second->layer;
		LayerMask layer2 = *updateLayer;
		if (layer1 == layer2) {

			physx::PxShape* shape;
			(*it).first->getShapes(&shape, 1);

			physx::PxFilterData* filterData;
			filterData = &shape->getSimulationFilterData();
			filterData->word1 = layer_list.at(layer2).LayerGroup;

			shape->setSimulationFilterData(*filterData);

			shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			shape->setQueryFilterData(*filterData);
			break;
		}
	}
}

void ModulePhysics::DeleteActor(physx::PxRigidActor* actor)
{
	if (actors.size() > 0)
	{
		for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = actors.begin(); it != actors.end(); ++it)
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
			GameObject* go = *it;
			DeleteActors((go));
			if (go->GetComponent<ComponentCollider>() != nullptr) {
				ComponentCollider* col = go->GetComponent<ComponentCollider>();
				physx::PxRigidActor* actor = col->GetActor();
				actors.erase(actor);
				col->Delete();
			}

			if (go->GetComponent<ComponentCharacterController>() != nullptr) {
				go->GetComponent<ComponentCharacterController>()->Delete();
			}
		}
	}

	if (go->GetComponent<ComponentCollider>() != nullptr) {
		ComponentCollider* col = go->GetComponent<ComponentCollider>();
		physx::PxRigidActor* actor = col->GetActor();
		actors.erase(actor);
		col->Delete();
	}

	if (go->GetComponent<ComponentCharacterController>() != nullptr) {
		go->GetComponent<ComponentCharacterController>()->Delete();
	}

	App->physics->mControllerManager->purgeControllers();
}

void ModulePhysics::OverlapSphere(float3 position, float radius, LayerMask layer, std::vector<uint>& objects)
{
	detected_objects = &objects;

	physx::PxOverlapHit hit[100];
	physx::PxOverlapBuffer hit_buffer(hit,100);       // [out] Overlap results
	const physx::PxSphereGeometry overlapShape(radius);			// [in] shape to test for overlaps
	const physx::PxTransform shapePose = physx::PxTransform(position.x, position.y, position.z);    // [in] initial shape pose (at distance=0)

	cache->fill(overlapShape, shapePose);

	physx::PxQueryFilterData filterData;
	filterData.data.word0 = App->physics->layer_list.at((int)layer).LayerGroup;
	filterData.flags |= physx::PxQueryFlag::eNO_BLOCK;

	bool status = cache->overlap(overlapShape, shapePose, hit_buffer, filterData);

	if (cache->getNbCachedShapes() > 0) {//DETECT CCT
		iter.layer = layer;
		cache->forEach((physx::PxVolumeCache::Iterator&)iter);
	}

	detected_objects = nullptr;
}

void UserIterator::processShapes(physx::PxU32 count, const physx::PxActorShape* actorShapePairs)
{
	int i = 0;
	for (physx::PxU32 i = 0; i < count; i++) {
		physx::PxRigidActor* actor = (physx::PxRigidActor * )actorShapePairs[i].actor;

		GameObject* GO = App->physics->actors[actor];
		if (GO) {
			if (layer == GO->layer) {
				if (App->physics->detected_objects) {
					App->physics->detected_objects->push_back(GO->GetUID());
				}
			}
		}
	}
}

physx::PxQueryHitType::Enum FilterCallback::preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags)
{
	// PT: ignore triggers
	if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)
		return physx::PxQueryHitType::eNONE;

	return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum FilterCallback::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit)
{
	return physx::PxQueryHitType::Enum();
}

const Broken::json& ModulePhysics::SaveStatus() const {
	//MYTODO: Added exception for Build because Build should never be enabled at start
	//maybe we should call SaveStatus on every panel
	static Broken::json config;

	config["gravity"] = gravity;
	config["staticFriction"] = mMaterial->getStaticFriction();
	config["dynamicFriction"] = mMaterial->getDynamicFriction();
	config["restitution"] = mMaterial->getRestitution();

	config["count"] = layer_list.size();

	for (uint i = 0; i < layer_list.size(); ++i) {
		Layer layer = layer_list.at(i);
		config["Layer" + std::to_string(i)]["Name"] = layer.name;
		config["Layer" + std::to_string(i)]["Layer"] = layer.layer;
		config["Layer" + std::to_string(i)]["Group"] = layer.LayerGroup;
		config["Layer" + std::to_string(i)]["GroupSize"] = layer.active_layers.size();
		config["Layer" + std::to_string(i)]["Active"] = layer.active;
		for (int j = 0; j < layer.active_layers.size(); ++j) {
			bool active = layer.active_layers.at(j);
			config["Layer" + std::to_string(i)][std::to_string(j)] = active;
		}

	}
	return config;
};

void ModulePhysics::LoadStatus(const Broken::json& file) {

	gravity = file[name]["gravity"].is_null() ? gravity : (float)file[name]["gravity"];
	materialDesc.x = file[name]["staticFriction"].is_null() ? materialDesc.x : (float)file[name]["staticFriction"];
	materialDesc.y = file[name]["dynamicFriction"].is_null() ? materialDesc.y : (float)file[name]["dynamicFriction"];
	materialDesc.z = file[name]["restitution"].is_null() ? materialDesc.z : (float)file[name]["restitution"];;

	int count = file[name]["count"].is_null() ? 0 : (int)file[name]["count"];

	if (count != 0) {
		for (uint i = 0; i < count; ++i) {
			Layer layer;
			if (file[name].find("Layer" + std::to_string(i)) != file[name].end()) {
				layer.name = file[name]["Layer" + std::to_string(i)]["Name"].get<std::string>();
				layer.layer = (LayerMask)file[name]["Layer" + std::to_string(i)]["Layer"];
				layer.LayerGroup = (physx::PxU32) file[name]["Layer" + std::to_string(i)]["Group"];
				layer.active = (physx::PxU32) file[name]["Layer" + std::to_string(i)]["Active"];

				int size = file[name]["Layer" + std::to_string(i)]["GroupSize"];
				layer.active_layers.resize(size, true);
				for (int j = 0; j < size; ++j) {
					if (file[name]["Layer" + std::to_string(i)].find(std::to_string(j)) != file[name]["Layer" + std::to_string(i)].end()) {
						layer.active_layers.at(j) = file[name]["Layer" + std::to_string(i)][std::to_string(j)];
					}
				}
			}
			layer_list.push_back(layer);
		}
		loaded = true;
	}
}
