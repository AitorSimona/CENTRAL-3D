#include "ComponentCollider.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModulePhysics.h"

using namespace physx;

ComponentCollider::ComponentCollider(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Collider) 
{
	ComponentTransform* transform = ContainerGO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	globalPosition = PxTransform(PxVec3(pos.x, pos.y, pos.z));
	PxBoxGeometry geometry(PxVec3(0.5f, 0.5f, 0.5f));

	box = PxCreateDynamic(*App->physics->mPhysics, globalPosition, geometry, *App->physics->mMaterial, 1.0f);

	App->physics->mScene->addActor(*box);
}

ComponentCollider::~ComponentCollider()
{
}

void ComponentCollider::Draw() const
{
}

void ComponentCollider::SetPosition()
{
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	float3 position(pos.x * localPosition.x, pos.y * localPosition.y, pos.z * localPosition.z);
	PxTransform globalPosition(PxVec3(position.x, position.y, position.z));

	box->setGlobalPose(globalPosition);
}

json ComponentCollider::Save() const
{
	return json();
}

void ComponentCollider::Load(json& node)
{
}
