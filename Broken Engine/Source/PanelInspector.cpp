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
#include "ComponentCollider.h"

#include "PanelShaderEditor.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ComponentScript.h"

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
			ImGui::NewLine();
			ImGui::Separator();
		}

		static ImGuiComboFlags flags = 0;

		const char* items[] = { "Default", "Mesh", "Mesh Renderer", "Dynamic RigidBody", "Collider" };
		static const char* item_current = items[0];

		ImGui::NewLine();

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

		// --- Handle drag & drop ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Resource* resource = App->resources->GetResource(UID, false);

				// MYTODO: Instance resource here, put it on scene (depending on resource)
				if (resource && resource->GetType() == Resource::ResourceType::SCRIPT)
				{
					resource = App->resources->GetResource(UID);
					ComponentScript* script = (ComponentScript*)Selected->AddComponent(Component::ComponentType::Script);
					script->AssignScript((ResourceScript*)resource);

				}
			}

			ImGui::EndDragDropTarget();
		}

		// --- Add here temporal conditions to know which component to add ---

		// MYTODO: Note currently you can not add the same type of component to a go (to be changed)

		if (item_current == "Mesh")
		{
			Selected->AddComponent(Component::ComponentType::Mesh);
		}

		if (item_current == "Mesh Renderer")
		{
			Selected->AddComponent(Component::ComponentType::MeshRenderer);
		}

		if (item_current == "Dynamic RigidBody")
		{
			Selected->AddComponent(Component::ComponentType::DynamicRigidBody);
		}

		if (item_current == "Collider")
		{
			Selected->AddComponent(Component::ComponentType::Collider);
		}

		item_current = items[0];
		// MYTODO: move this to the component itself

		// --- Material ---
		//if (Selected->GetComponent<ComponentMaterial>())
		//{
		//	CreateMaterialNode(*Selected);
		//	ImGui::Separator();
		//}

		// --- Camera ---
		//if (Selected->GetComponent<ComponentCamera>())
		//{
		//	CreateCameraNode(*Selected);
		//	ImGui::Separator();
		//}



		if(Startup)
			Startup = false;
	}

	ImGui::End();


	return true;
}

void PanelInspector::CreateGameObjectNode(GameObject & Selected) const
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
