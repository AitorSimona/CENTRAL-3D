#include "Application.h"
#include "ComponentDynamicRigidBody.h"
#include "ModulePhysics.h"

#include "Imgui/imgui.h"


ComponentDynamicRigidBody::ComponentDynamicRigidBody(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::DynamicRigidBody)
{
	/*if (!(rigidBody = ContainerGO->GetComponent<ComponentDynamicRigidBody>()->rigidBody))
		rigidBody = physx::PxCreateDynamic(*App->physics->mPhysics, PxTransform(), PxBoxGeometry(), *App->physics->mMaterial, 0.0f);*/

}

ComponentDynamicRigidBody::~ComponentDynamicRigidBody()
{

}

json ComponentDynamicRigidBody::Save() const
{
	return json();
}

void ComponentDynamicRigidBody::Load(json& node)
{
}

void ComponentDynamicRigidBody::CreateInspectorNode()
{
	ImGui::Checkbox("##Dynamic RigidBody", &GetActive()); ImGui::SameLine(); ImGui::Text("Dynamic RigidBody");

	ImGui::PushID("Mass"); ImGui::Text("Mass:"); ImGui::SameLine(); ImGui::InputFloat("", &mass); ImGui::PopID();
	ImGui::PushID("Density"); ImGui::Text("Density:"); ImGui::SameLine(); ImGui::InputFloat("", &density); ImGui::PopID();
	ImGui::PushID("Gravity"); ImGui::Text("Gravity:"); ImGui::SameLine(); ImGui::Checkbox("", &use_gravity); ImGui::PopID();
	ImGui::PushID("Kinematic"); ImGui::Text("Kinematic:"); ImGui::SameLine(); ImGui::Checkbox("", &is_kinematic); ImGui::PopID();
	ImGui::PushID("Linear Velocity"); ImGui::Text("Linear Velocity:"); ImGui::SameLine(); ImGui::PushItemWidth(50); ImGui::InputFloat("X", &linear_vel.x); ImGui::SameLine(); ImGui::InputFloat("Y", &linear_vel.y); ImGui::SameLine(); ImGui::InputFloat("Z", &linear_vel.z); ImGui::PopItemWidth(); ImGui::PopID();
	ImGui::PushID("Angular Velocity"); ImGui::Text("Angular Velocity:"); ImGui::SameLine(); ImGui::PushItemWidth(50); ImGui::InputFloat("X", &linear_vel.x); ImGui::SameLine(); ImGui::InputFloat("Y", &linear_vel.y); ImGui::SameLine(); ImGui::InputFloat("Z", &linear_vel.z); ImGui::PopItemWidth(); ImGui::PopID();
	ImGui::PushID("Linear Dumping"); ImGui::Text("Linear Dumping:"); ImGui::SameLine(); ImGui::InputFloat("", &linear_dumping); ImGui::PopID();
	ImGui::PushID("Angular Dumping"); ImGui::Text("Angular Dumping:"); ImGui::SameLine(); ImGui::InputFloat("", &angular_dumping); ImGui::PopID();
	


	if (ImGui::TreeNode("Constraints"))
	{
		ImGui::PushID("Freeze Position"); ImGui::Text("Freeze Position"); ImGui::SameLine(); ImGui::Checkbox("X", &freezePosition_X); ImGui::SameLine(); ImGui::Checkbox("Y", &freezePosition_Y); ImGui::SameLine(); ImGui::Checkbox("Z", &freezePosition_Z); ImGui::PopID();
		ImGui::PushID("Freeze Rotation"); ImGui::Text("Freeze Rotation"); ImGui::SameLine(); ImGui::Checkbox("X", &freeseRotation_X); ImGui::SameLine(); ImGui::Checkbox("Y", &freeseRotation_Y); ImGui::SameLine(); ImGui::Checkbox("Z", &freeseRotation_Z); ImGui::PopID();
		ImGui::TreePop();
	}

	if (rigidBody != nullptr) {
		SetMass(mass);
		SetDensity(density);
		UseGravity(use_gravity);
		SetKinematic(is_kinematic);
		/*SetLinearVelocity(linear_vel);
		SetAngularVelocity(angular_vel);
		SetLinearDamping(linear_dumping);
		SetAngularDamping(angular_dumping);*/
		FeezePosition_X(freezePosition_X);
		FeezePosition_Y(freezePosition_Y);
		FeezePosition_Z(freezePosition_Z);
		FreeseRotation_X(freeseRotation_X);
		FreeseRotation_Y(freeseRotation_Y);
		FreeseRotation_Z(freeseRotation_Z);
	}

}


