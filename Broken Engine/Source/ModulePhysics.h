#ifndef MODULE_PHYSICS_H_
#define MODULE_PHYSICS_H_
#include "Module.h"
#include "PhysX_3.4/Include/PxVolumeCache.h"

#include "Math.h"

namespace physx
{
	class PxPvd;
	class PxPvdSceneClient;
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
	class PxRigidStatic;
	class PxControllerManager;
	class PxRigidActor;
	class PxVolumeCache;
	class PxSimulationEventCallback;
	class PxActorShape;
	class PxQueryFilterCallback;

	typedef uint32_t PxU32;
	const float fixed_dt = (1.0f / 60.0f);
};

#define MAX_HITS 256

	enum LayerMask
	{
		LAYER_0 /*= (1 << 0)*/,
		LAYER_1 /*= (1 << 1)*/,
		LAYER_2 /*= (1 << 2)*/,
		LAYER_3 /*= (1 << 3)*/,
		LAYER_4 /*= (1 << 4)*/,
		LAYER_5 /*= (1 << 5)*/,
		LAYER_6 /*= (1 << 6)*/,
		LAYER_7 /*= (1 << 7)*/,
		LAYER_8 /*= (1 << 8)*/,
		LAYER_9 /*= (1 << 9)*/,
	};

struct Layer {
	std::string name;
	LayerMask layer;
	std::vector<bool> active_layers;
	physx::PxU32 LayerGroup;

	void UpdateLayerGroup() {
		physx::PxU32 ID = 0;
		for (int i = 0; i < active_layers.size(); ++i) //Return group of layers
		{
			bool active = active_layers.at(i);
			if (active)
				ID |= (1 << i);
		}
		LayerGroup = ID;
		
	}
};

struct BROKEN_API UserIterator : physx::PxVolumeCache::Iterator
{
	virtual void processShapes(physx::PxU32 count, const physx::PxActorShape* actorShapePairs);

	LayerMask layer; 
};

struct BROKEN_API FilterCallback : physx::PxQueryFilterCallback {
	virtual physx::PxQueryHitType::Enum preFilter(
		const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags);

	virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit);
};

BE_BEGIN_NAMESPACE
class GameObject;
class PhysxSimulationEvents;

class BROKEN_API ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(json& config) override;
	update_status Update(float dt) override;
	void FixedUpdate();

	void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 LayerMask, physx::PxU32 filterMask);

	//physx::PxFilterFlags customFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);


	bool CleanUp() override;

	void PlaneCollider(float posX, float posY, float posZ);
	void BoxCollider(float posX, float posY, float posZ);

	void SimulatePhysics(float dt, float speed = 1.0f);

	void addActor(physx::PxRigidActor* actor, GameObject* gameObject);

	void UpdateActor(physx::PxRigidActor* actor, LayerMask* LayerMask);

	void UpdateActors(LayerMask* updateLayer);

	void DeleteActor(physx::PxRigidActor* actor);

	void DeleteActors(GameObject* go = nullptr);

	void OverlapSphere(float3 position, float radius, LayerMask layer, std::vector<GameObject*>& objects);

	const Broken::json& SaveStatus() const;

	void LoadStatus(const Broken::json& file);

public:

	physx::PxPvd* mPvd = nullptr;
	physx::PxPvdSceneClient* pvdClient = nullptr;
	physx::PxFoundation* mFoundation = nullptr;
	physx::PxControllerManager* mControllerManager = nullptr;
	physx::PxPhysics* mPhysics = nullptr;
	physx::PxScene* mScene = nullptr;
	physx::PxMaterial* mMaterial = nullptr;
	physx::PxRigidStatic* plane = nullptr;
	std::vector<Layer> layer_list;
	std::map<physx::PxRigidActor*, GameObject*> actors;
	std::vector<GameObject*>* detected_objects;
	physx::PxVolumeCache* cache;
	UserIterator iter;

	FilterCallback filterCallback;

private:
	PhysxSimulationEvents* simulationEventsCallback = nullptr;
	float physAccumulatedTime = 0.0f;
};

BE_END_NAMESPACE
#endif
