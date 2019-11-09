#include "PanelInspector.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"

#include "mmgr/mmgr.h"

PanelInspector::PanelInspector(char * name) : Panel(name)
{
}

PanelInspector::~PanelInspector()
{
}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		GameObject* Selected = App->scene_manager->GetSelectedGameObjects();

		if (Selected == nullptr)
		{
			ImGui::End();
			return false;
		}

		// --- Game Object ---
		CreateGameObjectNode(*Selected);

		// --- Components ---

		if(Startup)
		ImGui::SetNextItemOpen(true);

		// --- Transform ---
		if (ImGui::TreeNode("Transform"))
		{
			CreateTransformNode(*Selected);
			ImGui::TreePop();
			ImGui::Separator();
		}

		// --- Mesh ---
		if (Selected->GetComponent<ComponentMesh>(Component::ComponentType::Mesh))
		{
			CreateMeshNode(*Selected);
			ImGui::Separator();
		}

		// --- Renderer ---
		if (Selected->GetComponent<ComponentRenderer>(Component::ComponentType::Renderer))
		{
			CreateRendererNode(*Selected);
			ImGui::Separator();
		}

		// --- Material ---
		if (Selected->GetComponent<ComponentMaterial>(Component::ComponentType::Material))
		{
			CreateMaterialNode(*Selected);
			ImGui::Separator();
		}

		if(Startup)
		Startup = false;
	}

	ImGui::End();


	return true;
}

inline void PanelInspector::CreateGameObjectNode(GameObject & Selected) const
{
	ImGui::BeginChild("child", ImVec2(0, 35), true);

	ImGui::Checkbox("##GOActive", &Selected.GetActive());
	ImGui::SameLine();

	// --- Game Object Name Setter ---
	static char GOName[100] = "";
	strcpy_s(GOName, 100, Selected.GetName().data());
	if (ImGui::InputText("", GOName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		Selected.SetName(GOName);

	ImGui::EndChild();
}

inline void PanelInspector::CreateTransformNode(GameObject& Selected) const
{
	ComponentTransform* transform = Selected.GetComponent<ComponentTransform>(Component::ComponentType::Transform);

	// --- Transform Position ---
	ImGui::Text("Position  ");
	ImGui::SameLine();

	float3 position = transform->GetPosition();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##PX", &position.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##PY", &position.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##PZ", &position.z, 0.005f);

	// --- Transform Rotation ---
	ImGui::Text("Rotation  ");
	ImGui::SameLine();

	float3 rotation = transform->GetRotation();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##RX", &rotation.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##RY", &rotation.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##RZ", &rotation.z, 0.005f);

	// --- Transform Scale ---
	ImGui::Text("Scale     ");
	ImGui::SameLine();

	float3 scale = transform->GetScale();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##SX", &scale.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##SY", &scale.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

	ImGui::DragFloat("##SZ", &scale.z, 0.005f);

	// --- Transform Set ---
	transform->SetPosition(position.x, position.y, position.z);
	transform->Scale(scale.x, scale.y, scale.z);
	transform->SetRotation(rotation);

}

inline void PanelInspector::CreateMeshNode(GameObject& Selected) const
{
	ComponentMesh* mesh = Selected.GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

	ImGui::Checkbox("##MeshActive", &mesh->GetActive());
	ImGui::SameLine();

	if (Startup)
		ImGui::SetNextItemOpen(true);

	if (ImGui::TreeNode("Mesh"))
	{
		std::string Triangle_count = "Triangles   ";
		Triangle_count.append(std::to_string(mesh->IndicesSize / 3));
		ImGui::Text(Triangle_count.data());

		ImGui::TreePop();
	}
}

inline void PanelInspector::CreateRendererNode(GameObject& Selected) const
{
	ComponentRenderer* renderer = Selected.GetComponent<ComponentRenderer>(Component::ComponentType::Renderer);

	ImGui::Checkbox("##RenActive", &renderer->GetActive());
	ImGui::SameLine();

	if (Startup)
		ImGui::SetNextItemOpen(true);

	if (ImGui::TreeNode("Mesh Renderer"))
	{

		ImGui::Checkbox("Vertex Normals", &renderer->draw_vertexnormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals  ", &renderer->draw_facenormals);
		ImGui::SameLine();
		ImGui::Checkbox("Checkers", &renderer->checkers);

		ImGui::TreePop();
	}
}

inline void PanelInspector::CreateMaterialNode(GameObject& Selected) const
{
	ComponentMaterial* material = Selected.GetComponent<ComponentMaterial>(Component::ComponentType::Material);

	ImGui::Checkbox("##MatActive", &material->GetActive());
	ImGui::SameLine();

	if (Startup)
		ImGui::SetNextItemOpen(true);

	if (ImGui::TreeNode("Material"))
	{

		// --- Print Texture Path ---
		std::string Path = "Path: ";
		Path.append(material->Texture_path);

		ImGui::Text(Path.data());

		// --- Texture Preview ---
		ImGui::Image((void*)(uint)&material->TextureID, ImVec2(150, 150));

		// --- Print Texture Width and Height ---
		ImGui::Text(std::to_string(material->Texture_width).data());
		ImGui::SameLine();
		ImGui::Text(std::to_string(material->Texture_height).data());

		ImGui::TreePop();
	}
}
