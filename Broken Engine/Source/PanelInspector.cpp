#include "PanelInspector.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleResourceManager.h"
#include "ModuleGui.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentCamera.h"

#include "PanelShaderEditor.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"

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
		GameObject* Selected = App->scene_manager->GetSelectedGameObject();

		if (Selected == nullptr)
		{
			ImGui::End();
			return false;
		}

		// --- Game Object ---
		CreateGameObjectNode(*Selected);

		// --- Components ---

		std::vector<Component*>* components = &Selected->GetComponents();

		for (std::vector<Component*>::const_iterator it = components->begin(); it != components->end(); ++it)
		{
			if (Startup)
				ImGui::SetNextItemOpen(true);

			(*it)->CreateInspectorNode();

			ImGui::Separator();
		}

		ImGui::Separator();

		static ImGuiComboFlags flags = 0;

		const char* items[] = { "Default", "ComponentMesh", "ComponentMeshRenderer" };
		static const char* item_current = items[0];

		// --- Add component ---
		if (ImGui::BeginCombo("##Components Combo", "Add Component", flags)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
					item_current = items[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
			ImGui::EndCombo();
		}

		// --- Add here temporal conditions to know which component to add ---

		// MYTODO: Note currently you can not add the same type of component to a go (to be changed)

		if (item_current == "ComponentMesh")
		{
			Selected->AddComponent(Component::ComponentType::Mesh);
		}

		if (item_current == "ComponentMeshRenderer")
		{
			Selected->AddComponent(Component::ComponentType::MeshRenderer);
		}

		item_current = items[0];


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

	ImGui::SameLine();

	if(ImGui::Checkbox("Static", &Selected.Static))
	App->scene_manager->SetStatic(&Selected);

	ImGui::EndChild();
}


inline void PanelInspector::CreateMaterialNode(GameObject& Selected) const
{
	//ComponentMaterial* material = Selected.GetComponent<ComponentMaterial>();

	//ImGui::Checkbox("##MatActive", &material->GetActive());
	ImGui::SameLine();

	if (Startup)
		ImGui::SetNextItemOpen(true);

	//if (ImGui::TreeNode("Material"))
	//{
	//	static ImGuiComboFlags flags = 0;

	//	ImGui::Text("Shader");
	//	ImGui::SameLine();

	//	std::map<std::string, ResourceShader*>* shaders = App->resources->GetShaders();

	//	const char* item_current = material->resource_material->shader->name.data();       
	//	if (ImGui::BeginCombo("##Shader", item_current, flags))
	//	{
	//		for (std::map<std::string, ResourceShader*>::iterator it = shaders->begin(); it != shaders->end(); ++it)
	//		{
	//			bool is_selected = (item_current == it->first);

	//			if (ImGui::Selectable(it->second->name.data(), is_selected))
	//			{
	//				item_current = it->second->name.data();
	//				material->resource_material->shader = it->second;
	//				material->resource_material->shader->GetAllUniforms(material->resource_material->uniforms);
	//			}
	//			if (is_selected)
	//				ImGui::SetItemDefaultFocus();
	//		}

	//		ImGui::EndCombo();
	//	}


	//	// --- Print Texture Path ---
	//	std::string Path = "Path: ";
	//	Path.append(material->resource_material->resource_diffuse->Texture_path);

	//	ImGui::Text(Path.data());

	//	// --- Texture Preview ---
	//	ImGui::Image((void*)(uint)&material->resource_material->resource_diffuse->buffer_id, ImVec2(150, 150));
	//	ImGui::Text("Instances:");
	//	ImGui::SameLine();
	//	ImGui::Text(std::to_string(material->resource_material->resource_diffuse->instances).data());

	//	// --- Print Texture Width and Height ---
	//	ImGui::Text(std::to_string(material->resource_material->resource_diffuse->Texture_width).data());
	//	ImGui::SameLine();
	//	ImGui::Text(std::to_string(material->resource_material->resource_diffuse->Texture_height).data());

	//	ImGui::Separator();

	//	ImGui::Text("Shader Uniforms");

	//	App->gui->panelShaderEditor->DisplayAndUpdateUniforms(material->resource_material);

	//	ImGui::TreePop();
	//}
}

inline void PanelInspector::CreateCameraNode(GameObject & Selected) const
{
	if (Startup)
		ImGui::SetNextItemOpen(true);

	ComponentCamera* camera = Selected.GetComponent<ComponentCamera>();

	if (ImGui::TreeNode("Camera"))
	{
		if (ImGui::Checkbox("Active Camera", &camera->active_camera))
			camera->active_camera ? App->renderer3D->SetActiveCamera(camera) : App->renderer3D->SetActiveCamera(nullptr);


		if (ImGui::Checkbox("Culling Camera", &camera->culling))
			camera->culling ? App->renderer3D->SetCullingCamera(camera) : App->renderer3D->SetCullingCamera(nullptr);

		ImGui::Text("FOV");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.15f);

		float fov = camera->GetFOV();

		ImGui::DragFloat("##FOV", &fov, 0.005f);

		if (fov != camera->GetFOV())
			camera->SetFOV(fov);

		ImGui::TreePop();
	}
	
}

