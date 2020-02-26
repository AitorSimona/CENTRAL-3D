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


ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO,Component::ComponentType::Mesh)
{
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

	// --- Store path to component file ---
	node["Resources"]["ResourceMesh"] = std::string(resource_mesh->GetResourceFile());

	return node;
}

void ComponentMesh::Load(json& node)
{
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
	ImGui::Checkbox("##MeshActive", &GetActive());
	ImGui::SameLine();

	if (resource_mesh && ImGui::TreeNode("Mesh"))
	{
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
	if (resource_def_mesh == nullptr)
	{
		resource_def_mesh = new ResourceMesh(App->GetRandom().Int(), resource_mesh->GetResourceFile());

		resource_def_mesh->IndicesSize = resource_mesh->IndicesSize;
		resource_def_mesh->Indices = new uint[resource_def_mesh->IndicesSize];
		memcpy(resource_def_mesh->Indices, resource_mesh->Indices, resource_def_mesh->IndicesSize * sizeof(uint));

		resource_def_mesh->VerticesSize = resource_mesh->VerticesSize;
		resource_def_mesh->vertices = new Vertex[resource_mesh->VerticesSize];

		// --- Normals ---
		for (uint i = 0; i < resource_def_mesh->VerticesSize / 3; ++i)
		{
			resource_def_mesh->vertices[i].normal[0] = 0.0f;
			resource_def_mesh->vertices[i].normal[1] = 0.0f;
			resource_def_mesh->vertices[i].normal[2] = 0.0f;
		}

		// --- Face Normals ---
		for (uint i = 0; i < resource_def_mesh->VerticesSize / 3; ++i)
		{
			resource_def_mesh->vertices[i].position[0] = 0.0f;
			resource_def_mesh->vertices[i].position[1] = 0.0f;
			resource_def_mesh->vertices[i].position[2] = 0.0f;
		}

		// SHADERS????????????????????????????????????????????????????????????????????????????
		uint idx = App->renderer3D->CreateBufferFromData(resource_def_mesh->VBO, resource_def_mesh->VerticesSize, resource_def_mesh->vertices);

		glGenBuffers(1, (GLuint*)&(idx));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * resource_def_mesh->VerticesSize, resource_def_mesh->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (std::vector<ComponentBone*>::iterator it = bones.begin(); it != bones.end(); it++)
		{
			ResourceBone* r_bone = (*it)->res_bone;

			float4x4 mat = (*it)->GetSkeletonTransform();
			mat = GO->GetComponent<ComponentTransform>()->GetLocalTransform() * mat;
			mat = mat * r_bone->matrix;

			for (uint i = 0; i < r_bone->NumWeights; i++)
			{
				uint index = r_bone->index_weight[i];
				float3 tmp = { 0, 0, 0 };
				float3 _vertex = mat.TransformPos(tmp);

				//float3 tmp = res_mesh->vertex[index];

				resource_def_mesh->vertices[index].position[0] += _vertex.x * r_bone->weight[i];
				resource_def_mesh->vertices[index].position[1] += _vertex.y * r_bone->weight[i];
				resource_def_mesh->vertices[index].position[2] += _vertex.z * r_bone->weight[i];

				if ((resource_mesh->VerticesSize / 3) > 0);
				{
					float3 aux = { resource_def_mesh->vertices[index].normal[0], resource_def_mesh->vertices[index].normal[1], resource_def_mesh->vertices[index].normal[3] };
					_vertex = mat.TransformPos(aux);
					resource_def_mesh->vertices[index].position[0] += _vertex.x * r_bone->weight[i];
					resource_def_mesh->vertices[index].position[1] += _vertex.y * r_bone->weight[i];
					resource_def_mesh->vertices[index].position[2] += _vertex.z * r_bone->weight[i];

					float3 aux_ = { resource_def_mesh->vertices[index].position[0], resource_def_mesh->vertices[index].position[1], resource_def_mesh->vertices[index].position[2] };
					_vertex = mat.TransformPos(aux_);
					resource_def_mesh->vertices[index].normal[0] += _vertex.x * r_bone->weight[i];
					resource_def_mesh->vertices[index].normal[1] += _vertex.y * r_bone->weight[i];
					resource_def_mesh->vertices[index].normal[2] += _vertex.z * r_bone->weight[i];
				}
			}
		}
	}
}

