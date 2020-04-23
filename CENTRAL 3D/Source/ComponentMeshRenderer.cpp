#include "ComponentMeshRenderer.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "Color.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTimeManager.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "ResourceMesh.h"
#include "ResourceShader.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"

#include "ImporterMeta.h"
#include "ImporterMaterial.h"
#include "ResourceMeta.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"


ComponentMeshRenderer::ComponentMeshRenderer(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::MeshRenderer)
{
	material = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID());
	name = "MeshRenderer";
}


ComponentMeshRenderer::~ComponentMeshRenderer()
{
	if (material && material->IsInMemory())
	{
		material->Release();
		material->RemoveUser(GO);
	}
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::DrawComponent()
{
	RenderMeshFlags flags = texture;

	ComponentMesh* cmesh = GO->GetComponent<ComponentMesh>();

	if (App->scene_manager->GetSelectedGameObject() && App->scene_manager->GetSelectedGameObject()->GetUID() == GO->GetUID())
		flags |= selected;

	if (checkers)
		flags |= RenderMeshFlags_::checkers;

	if (cmesh && cmesh->resource_mesh && material)
	{
		App->renderer3D->DrawMesh(GO->GetComponent<ComponentTransform>()->GetGlobalTransform(), cmesh->resource_mesh, material, flags);
		DrawNormals(*cmesh->resource_mesh, *GO->GetComponent<ComponentTransform>());
	}
}

void ComponentMeshRenderer::DrawNormals(const ResourceMesh& mesh, const ComponentTransform& transform) const
{
	float3 origin = float3::zero;
	float3 end = float3::zero;
	Color color(255, 255, 0);
	float4x4 transf = transform.GetGlobalTransform();

	// --- Draw vertex normals ---

	if (draw_vertexnormals && mesh.vertices->normal)
	{
		for (uint i = 0; i < mesh.IndicesSize; ++i)
		{
			// --- Normals ---
			origin = float3(mesh.vertices[mesh.Indices[i]].position[0], mesh.vertices[mesh.Indices[i]].position[1], mesh.vertices[mesh.Indices[i]].position[2]);
			end = float3(mesh.vertices[mesh.Indices[i]].position[0] + mesh.vertices[mesh.Indices[i]].normal[0] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[1] + mesh.vertices[mesh.Indices[i]].normal[1] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[2] + mesh.vertices[mesh.Indices[i]].normal[2] * NORMAL_LENGTH);

			App->renderer3D->DrawLine(transf, origin, end, color);
		}
	}

	// --- Draw Face Normals ---

	if (draw_facenormals)
	{
		Triangle face;
		float3 face_center;
		float3 face_normal;

		// --- Compute face normals ---
		for (uint j = 0; j < mesh.IndicesSize / 3; ++j)
		{
			face.a = float3(mesh.vertices[mesh.Indices[j * 3]].position);
			face.b = float3(mesh.vertices[mesh.Indices[(j * 3) + 1]].position);
			face.c = float3(mesh.vertices[mesh.Indices[(j * 3) + 2]].position);

			face_center = face.Centroid();

			face_normal = Cross(face.b - face.a, face.c - face.b);

			face_normal.Normalize();

			origin = float3(face_center.x, face_center.y, face_center.z);
			end = float3(face_center.x + face_normal.x * NORMAL_LENGTH, face_center.y + face_normal.y * NORMAL_LENGTH, face_center.z + face_normal.z * NORMAL_LENGTH);

			App->renderer3D->DrawLine(transf, origin, end, color);
		}
	}
}

json ComponentMeshRenderer::Save() const
{
	json node;
	node["Active"] = this->active;

	node["Resources"]["ResourceMaterial"]["path"];

	if (material)
		node["Resources"]["ResourceMaterial"]["path"] = std::string(material->GetResourceFile());

	return node;
}

void ComponentMeshRenderer::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];

	std::string mat_path = node["Resources"]["ResourceMaterial"]["path"].is_null() ? "0" : node["Resources"]["ResourceMaterial"]["path"];

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	if (IMeta)
	{
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(mat_path.c_str());

		if (material && meta)
			material->Release();

		if (meta)
			material = (ResourceMaterial*)App->resources->GetResource(meta->GetUID());

		// --- We want to be notified of any resource event ---
		if (material)
			material->AddUser(GO);
	}
}

void ComponentMeshRenderer::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---

	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (material && UID == material->GetUID())
			material = (ResourceMaterial*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (material && UID == material->GetUID())
			material = nullptr;
		break;

	default:
		break;
	}
}

void ComponentMeshRenderer::CreateInspectorNode()
{
	ImGui::Checkbox("Vertex Normals", &draw_vertexnormals);
	ImGui::SameLine();
	ImGui::Checkbox("Face Normals  ", &draw_facenormals);
	ImGui::SameLine();
	ImGui::Checkbox("Checkers", &checkers);


	ImGui::NewLine();
	ImGui::Separator();
	ImGui::PushID("Material");

	// --- Material node ---
	if (material)
	{
		bool is_default = material->GetUID() == App->resources->DefaultMaterial->GetUID();

		// --- Mat preview
		ImGui::Image((void*)(uint)material->GetPreviewTexID(), ImVec2(30, 30));
		ImGui::SameLine();

		if (ImGui::TreeNode(material->GetName()))
		{
			static ImGuiComboFlags flags = 0;

			ImGui::Text("Shader");
			ImGui::SameLine();

			if (material->shader)
			{
				const char* item_current = material->shader->GetName();
				if (ImGui::BeginCombo("##Shader", item_current, flags))
				{
					for (std::map<uint, ResourceShader*>::iterator it = App->resources->shaders.begin(); it != App->resources->shaders.end(); ++it)
					{
						bool is_selected = (item_current == it->second->GetName());

						if (ImGui::Selectable(it->second->GetName(), is_selected))
						{
							item_current = it->second->GetName();
							material->shader = it->second;
							material->shader->GetAllUniforms(material->uniforms);
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			if (!is_default)
			{
				// --- Uniforms ---
				material->DisplayAndUpdateUniforms();

				// --- Color ---
				ImGui::Separator();
				ImGui::ColorEdit4("##AmbientColor", (float*)&material->color, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("Ambient Color");


				ImGui::Separator();

				// --- Environment mapping ---
				if (ImGui::Button("None"))
				{
					material->reflective = false;
					material->refractive = false;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("Reflective", material->reflective))
				{
					material->reflective = true;
					material->refractive = false;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("Refractive", material->refractive))
				{
					material->reflective = false;
					material->refractive = true;
				}

				ImGui::Separator();

				// --- Print Texture Width and Height (Diffuse) ---
				uint textSizeX = 0, textSizeY = 0;
				ImGui::NewLine();
				if (material->resource_diffuse)
				{
					textSizeX = material->resource_diffuse->Texture_width;
					textSizeY = material->resource_diffuse->Texture_height;
				}

				ImGui::Text(std::to_string(textSizeX).c_str());
				ImGui::SameLine();
				ImGui::Text(std::to_string(textSizeY).c_str());

				// --- Texture Preview ---
				if (material->resource_diffuse)
					ImGui::ImageButton((void*)(uint)material->resource_diffuse->GetPreviewTexID(), ImVec2(20, 20));
				else
					ImGui::ImageButton(NULL, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 2);

				// --- Handle drag & drop (Diffuse Texture) ---
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
					{
						uint UID = *(const uint*)payload->Data;
						Resource* resource = App->resources->GetResource(UID, false);

						if (resource && resource->GetType() == Resource::ResourceType::TEXTURE)
						{
							if (material->resource_diffuse)
								material->resource_diffuse->Release();

							material->resource_diffuse = (ResourceTexture*)App->resources->GetResource(UID);

							// --- Save material so we update path to texture ---
							ImporterMaterial* IMat = App->resources->GetImporter<ImporterMaterial>();

							if (IMat)
								IMat->Save(material);
						}
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::Text("Albedo");

				ImGui::SameLine();
				if (ImGui::Button("Unuse", { 43, 18 }) && material->resource_diffuse)
				{
					material->resource_diffuse->RemoveUser(GetContainerGameObject());
					material->resource_diffuse->Release();
					material->resource_diffuse = nullptr;
				}

			}

			ImGui::TreePop();

			if (ImGui::Button("Unuse Material"))
			{
				if (material && material->IsInMemory())
				{
					material->Release();
					material->RemoveUser(GO);
					material = nullptr;
				}
			}
		}
	}
	else
	{
		ImGui::ImageButton(NULL, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 2);

		// --- Handle drag & drop ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Resource* resource = App->resources->GetResource(UID, false);

				if (resource && resource->GetType() == Resource::ResourceType::MATERIAL)
				{
					material = (ResourceMaterial*)App->resources->GetResource(UID);
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		ImGui::Text("Drop Material");
	}

	ImGui::PopID();
}

