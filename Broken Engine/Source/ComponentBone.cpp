#include "ComponentBone.h"

// -- Modules --
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"

// -- Resources --
#include "ResourceShader.h"
#include "ResourceBone.h"

// -- Components --
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "Imgui/imgui.h"
#include "OpenGL.h"

using namespace Broken;

ComponentBone::ComponentBone(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Bone)
{
	name = "Bone";
}

ComponentBone::~ComponentBone()
{
	if (res_bone && res_bone->IsInMemory())
	{
		res_bone->Release();
		res_bone->RemoveUser(GO);
	}
}

void ComponentBone::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentBone::DrawComponent()
{
	if (GO->parent != nullptr && GO->parent->GetComponent<ComponentBone>() != nullptr && GO->GetAnimGO(GetHipBone()->GO)
		&& GO->GetAnimGO(GetHipBone()->GO)->GetComponent<ComponentAnimation>()->draw_bones)
	{
		float4x4 child_matrix = GetBoneTransform();
		float4x4 parent_matrix = GO->parent->GetComponent<ComponentBone>()->GetBoneTransform();
		float3 child_pos = float3(child_matrix.At(0, 3), child_matrix.At(1, 3), child_matrix.At(2, 3));
		float3 parent_pos = float3(parent_matrix.At(0, 3), parent_matrix.At(1, 3), parent_matrix.At(2, 3));

		App->renderer3D->DrawLine(child_matrix, child_pos, parent_pos, White);
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
	ComponentBone* pBone = GO->parent->GetComponent<ComponentBone>();

	if (pBone == nullptr)
	{
		return this;
	}
	else
		return pBone->GetHipBone();
}

json ComponentBone::Save() const
{
	json node;
	node["Active"] = this->active;

	node["Resources"]["ResourceBone"];

	// --- Store path to component file ---
	if (res_bone)
		node["Resources"]["ResourceBone"] = std::string(res_bone->GetResourceFile());


	return node;
}

void ComponentBone::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];

	std::string path = node["Resources"]["ResourceBone"].is_null() ? "0" : node["Resources"]["ResourceBone"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (res_bone)
		res_bone->Release();

	res_bone = (ResourceBone*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (res_bone)
		res_bone->AddUser(GO);
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

void ComponentBone::CreateInspectorNode()
{

	if (res_bone)
	{
		ImGui::Text("Name: %s", res_bone->GetName());
	}

}
