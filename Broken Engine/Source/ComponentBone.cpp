#include "Application.h"
#include "ComponentBone.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"

#include "OpenGL.h"

#include "Color.h"

ComponentBone::ComponentBone(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Bone)
{

}

ComponentBone::~ComponentBone()
{
	if (res_bone && res_bone->IsInMemory())
	{
		res_bone->Release();
		res_bone->RemoveUser(GO);
	}
}

void ComponentBone::DebugDrawBones()
{
	if (GO->parent != nullptr && GO->parent->GetComponent<ComponentBone>() != nullptr
		&& GO->GetComponent<ComponentAnimation>()->draw_bones)
	{
		float4x4 child_matrix = GetBoneTransform();
		float4x4 parent_matrix = GO->parent->GetComponent<ComponentBone>()->GetBoneTransform();
		float3 child_pos = float3(child_matrix.At(0, 3), child_matrix.At(1, 3), child_matrix.At(2, 3));
		float3 parent_pos = float3(parent_matrix.At(0, 3), parent_matrix.At(1, 3), parent_matrix.At(2, 3));

		glLineWidth(5.0f);
		glBegin(GL_LINES);

		//Yellow
		glColor4f(1.0, 1.0, 0.0, 1.0);

		glVertex3f(child_pos.x, child_pos.y, child_pos.z);
		glVertex3f(parent_pos.x, parent_pos.y, parent_pos.z);


		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

float4x4 ComponentBone::GetBoneTransform() const
{
	return GO->GetComponent<ComponentTransform>()->GetGlobalTransform();
}

float4x4 ComponentBone::GetSkeletonTransform()
{
	return GO->GetAnimGO(GetHipBone()->GO)->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted() * GO->GetComponent<ComponentTransform>()->GetGlobalTransform();
}


ComponentBone* ComponentBone::GetHipBone()
{
	ComponentBone* pBone = GO->GetComponent<ComponentBone>();

	if (pBone == nullptr)
	{
		return this;
	}
	else
		return pBone->GetHipBone();
}

json ComponentBone::Save() const
{
	return json();
}

void ComponentBone::Load(json& node)
{
}

void ComponentBone::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (res_bone && UID == res_bone->GetUID())
			res_bone = (ResourceBone*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (res_bone && UID == res_bone->GetUID())
			res_bone = nullptr;
		break;

	default:
		break;
	}
}