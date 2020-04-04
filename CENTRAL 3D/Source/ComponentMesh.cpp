#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO,Component::ComponentType::Mesh)
{
	name = "Mesh";
}

ComponentMesh::~ComponentMesh()
{
	if (resource_mesh && resource_mesh->IsInMemory())
	{
		resource_mesh->Release();
		resource_mesh->RemoveUser(GO);
	}
}

const AABB & ComponentMesh::GetAABB() const
{
	if (resource_mesh)
		return resource_mesh->aabb;
	else
		return AABB();
}

json ComponentMesh::Save() const
{
	json node;
	node["Resources"]["ResourceMesh"]["path"];

	// --- Store path to component file ---
	if (resource_mesh)
	{
		node["Resources"]["ResourceMesh"]["PreviewTexture"] = resource_mesh->previewTexPath; // for now we save this here 
		node["Resources"]["ResourceMesh"]["path"] = std::string(resource_mesh->GetResourceFile());
	}

	return node;
}

void ComponentMesh::Load(json& node)
{
	std::string path = node["Resources"]["ResourceMesh"]["path"].is_null() ? "-1" : node["Resources"]["ResourceMesh"]["path"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (resource_mesh)
		resource_mesh->Release();

	if(std::stoi(path) != -1)
		resource_mesh = (ResourceMesh*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (resource_mesh)
		resource_mesh->AddUser(GO);
}

void ComponentMesh::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (resource_mesh && UID == resource_mesh->GetUID())
			resource_mesh = (ResourceMesh*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (resource_mesh && UID == resource_mesh->GetUID())
			resource_mesh = nullptr;
		break;

	default:
		break;
	}
}

void ComponentMesh::CreateInspectorNode()
{
	if (resource_mesh)
	{
		std::string Triangle_count = "Triangles   ";
		Triangle_count.append(std::to_string(resource_mesh->IndicesSize / 3));
		ImGui::Text(Triangle_count.data());
	}

	//ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	//ImGui::SameLine();
	//if (ImGui::Button("Delete")) {
	//	GO->RemoveComponent(Component::ComponentType::Mesh);
	//}
}

