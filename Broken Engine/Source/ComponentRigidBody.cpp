#include "Application.h"
#include "ComponentRigidBody.h"
#include "ModulePhysics.h"


ComponentRigidBody::ComponentRigidBody(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::RigidBody)
{
	if (!(rigidBody = ContainerGO->GetComponent<ComponentRigidBody>()->rigidBody))
		rigidBody = physx::PxCreateDynamic(*App->physics->mPhysics,PxTransform(), PxBoxGeometry(), *App->physics->mMaterial,0.0f);

}

ComponentRigidBody::~ComponentRigidBody()
{

}

json ComponentRigidBody::Save() const
{
	return json();
}

void ComponentRigidBody::Load(json& node)
{
}

void ComponentRigidBody::CreateInspectorNode()
{
}


