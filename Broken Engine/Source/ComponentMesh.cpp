#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"

#include "ComponentBone.h"
#include "ComponentTransform.h"

#include "Math.h"

#include "GameObject.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Mesh) {
}

ComponentMesh::~ComponentMesh() {
	if (resource_mesh && resource_mesh->IsInMemory()) {
		resource_mesh->Release();
		resource_mesh->RemoveUser(GO);
	}
}

const AABB& ComponentMesh::GetAABB() const {
	if (resource_mesh)
		return resource_mesh->aabb;
	else
		return AABB();
}

json ComponentMesh::Save() const {
	json node;
	node["Resources"]["ResourceMesh"];

	// --- Store path to component file ---
	if(resource_mesh)
	node["Resources"]["ResourceMesh"] = std::string(resource_mesh->GetResourceFile());

	return node;
}

void ComponentMesh::Load(json& node) {
	std::string path = node["Resources"]["ResourceMesh"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (resource_mesh)
		resource_mesh->Release();

	resource_mesh = (ResourceMesh*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (resource_mesh)
		resource_mesh->AddUser(GO);
}

void ComponentMesh::ONResourceEvent(uint UID, Resource::ResourceNotificationType type) {
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type) {
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

void ComponentMesh::CreateInspectorNode() {
	ImGui::Checkbox("##MeshActive", &GetActive());
	ImGui::SameLine();

	if (resource_mesh && ImGui::TreeNode("Mesh")) {
		std::string Triangle_count = "Triangles   ";
		Triangle_count.append(std::to_string(resource_mesh->IndicesSize / 3));
		ImGui::Text(Triangle_count.data());

		//ImGui::Text("Instances:");
		//ImGui::SameLine();
		//ImGui::Text(std::to_string(mesh->resource_mesh->instances).data());

		ImGui::TreePop();
	}

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		GO->RemoveComponent(Component::ComponentType::Mesh);
	}
}

void ComponentMesh::AddBone(ComponentBone* bone)
{
	for (int i = 0; i < bones.size(); i++)
		if (bones[i] == bone)
			return;

	bones.push_back(bone);
}

void ComponentMesh::UpdateDefMesh()
{

	for (uint i = 0; i < resource_mesh->VerticesSize; ++i)
	{
		resource_mesh->vertices[i].animPos_offset[0] = 0.0f;
		resource_mesh->vertices[i].animPos_offset[1] = 0.0f;
		resource_mesh->vertices[i].animPos_offset[2] = 0.0f;
	}

	for (std::vector<ComponentBone*>::iterator it = bones.begin(); it != bones.end(); ++it)
	{
		ResourceBone* r_bone = (*it)->res_bone;

		float4x4 mat = (*it)->GetSkeletonTransform();
		mat = GO->GetComponent<ComponentTransform>()->GetLocalTransform().Inverted() * mat;
		mat = mat * r_bone->matrix;

		for (uint i = 0; i < r_bone->NumWeights; i++)
		{

			uint index = r_bone->index_weight[i];

			// -- This is the original vertex
			float3 tmp = { resource_mesh->vertices[index].position[0],
						   resource_mesh->vertices[index].position[1],
						   resource_mesh->vertices[index].position[2]};
					   			 


			float3 _vertex = mat.TransformPos(tmp.x, tmp.y, tmp.z);//LINE OF DEATH

			resource_mesh->vertices[index].animPos_offset[0] += _vertex.x * r_bone->weight[i];
			resource_mesh->vertices[index].animPos_offset[1] += _vertex.y * r_bone->weight[i];
			resource_mesh->vertices[index].animPos_offset[2] += _vertex.z * r_bone->weight[i];

			/*if ((resource_mesh->VerticesSize / 3) > 0);
			{
				float3 aux_ = { resource_mesh->vertices[index].position[0],
								resource_mesh->vertices[index].position[1],
								resource_mesh->vertices[index].position[2] };
				_vertex = mat.TransformPos(aux_);
				resource_def_mesh->vertices[index].normal[0] += _vertex.x * r_bone->weight[i];
				resource_def_mesh->vertices[index].normal[1] += _vertex.y * r_bone->weight[i];
				resource_def_mesh->vertices[index].normal[2] += _vertex.z * r_bone->weight[i];
			}*/
		}
	}

	// --- Bind it ---
	glBindVertexArray(resource_mesh->VAO);

	// Bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->VBO);

	// --- Set all vertex attribute pointers ---

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * resource_mesh->VerticesSize, resource_mesh->vertices, GL_DYNAMIC_DRAW);

	// --- Unbind VAO and VBO ---
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
