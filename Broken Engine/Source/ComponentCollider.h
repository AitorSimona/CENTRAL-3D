#ifndef __COMPONENT_COLLIDER_H__
#define __COMPONENT_COLLIDER_H__

#include "Component.h"
#include "Math.h"

namespace physx
{
	class PxRigidActor;
	class PxRigidStatic;
	class PxTransform;
	class PxShape;
	class PxConvexMesh;
	class PxTriangleMesh; 
	class PxVec3;
	typedef uint16_t PxU16;
}

BE_BEGIN_NAMESPACE


class ResourceMesh;
class ComponentDynamicRigidBody;
class ComponentTransform;
struct Vertex;

class BROKEN_API ComponentCollider : public Component
{

public:
	enum class COLLIDER_TYPE
	{
		NONE,
		BOX,
		SPHERE,
		CAPSULE,
		MESH
	};

public:

	friend class Callbacks;

	ComponentCollider(GameObject* ContainerGO);
	virtual ~ComponentCollider();

	void Update() override;
	void Enable() override;
	void Disable() override;

	void UpdateCollider();

	void DrawComponent() override;

	void UpdateLocalMatrix();

	void UpdateTransformByRigidBody(ComponentDynamicRigidBody* dynamicRB, ComponentTransform* cTransform, physx::PxTransform* globalPos = nullptr);


	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	void GetMesh();

	void CreateCollider(ComponentCollider::COLLIDER_TYPE type, bool createAgain = false);

	void CreateRigidbody();

	static inline Component::ComponentType GetType() { return Component::ComponentType::Collider; };

	float4x4 GetGlobalMatrix() { return globalMatrix; }

	physx::PxRigidActor*					GetActor();
	void									UpdateActorLayer(const int* layerMask);
	void									Delete();

private:
	template<class Geometry>
	void CreateRigidbody(Geometry geometry, physx::PxTransform position);
	template <class Geometry>
	bool HasDynamicRigidBody(Geometry geometry, physx::PxTransform transform);

public:
	COLLIDER_TYPE type = COLLIDER_TYPE::NONE;
	ResourceMesh* mesh = nullptr;
	ResourceMesh* current_mesh = nullptr;
	ResourceMesh* dragged_mesh = nullptr;
	bool editCollider = false;
	bool updateValues = false;
	float3 centerPosition = float3::zero;
	float3 originalSize = float3::one;
	physx::PxRigidStatic* rigidStatic = nullptr;
	float3 offset = float3::zero;
	int colliderType = 0;
	bool hasBeenDeactivated = false;
	bool isTrigger = false;
	bool isConvex = false;
	Quat dragged_rot = Quat::identity;
	float3 dragged_scale = float3::one;
	int dragged_UID = 0;


private:
	physx::PxShape* shape = nullptr;
	physx::PxConvexMesh* convex_mesh = nullptr;
	physx::PxTriangleMesh* triangle_mesh = nullptr;
	float3 colliderSize = float3(1, 1, 1);
	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;
	float radius = 1.0f, height = 1.0f;
	int lastIndex = -1;
	float3 tmpScale = float3::one;
	bool firstCreation = false;
	bool toPlay = false;
	bool draw = false;
	bool localMesh = false;
};


BE_END_NAMESPACE
#endif __COMPONENT_COLLIDER_H__
