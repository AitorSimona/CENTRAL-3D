#include "PanelInspector.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleGui.h"

#include "GameObject.h"
#include "Components.h"
#include "PanelProject.h"

#include "Imgui/imgui.h"


#include "mmgr/mmgr.h"

PanelInspector::PanelInspector(char* name) : Panel(name)
{
}

PanelInspector::~PanelInspector()
{
}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing ;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		GameObject* Selected = App->scene_manager->GetSelectedGameObject();
		Resource* SelectedRes = App->gui->panelProject->GetSelected();

		if (Selected != nullptr)
		{

			// --- Game Object ---
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

			ImGui::BeginChildFrame(Selected->GetUID(),ImVec2(ImGui::GetWindowWidth(),50), ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);
			CreateGameObjectNode(*Selected);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			ImGui::EndChildFrame();

			ImGui::PopStyleColor();

			// --- Components ---

			std::vector<Component*>* components = &Selected->GetComponents();
			uint i = 1;


			for (std::vector<Component*>::const_iterator it = components->begin(); it != components->end(); ++it)
			{
				if (Startup)
					ImGui::SetNextItemOpen(true);

				if (*it)
				{
					ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
					
					if (ImGui::BeginChildFrame(Selected->GetUID() + i, ImVec2(ImGui::GetWindowWidth(), 20), ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse))
					{
						if (i == 1)
							ImGui::PopStyleVar();
						

						ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 1));

						std::string a = "##Active";

						if((*it)->name != "Transform")
							ImGui::Checkbox((a + (*it)->name).c_str(), &(*it)->GetActive());


						ImGui::SameLine();
						ImGui::Text(((*it)->name).c_str());

						ImGui::PopStyleVar();

						ImGui::EndChildFrame();
		
					}
					
					ImGui::SameLine();

					if (ImGui::TreeNodeEx((*it)->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanFullWidth))
					{
						(*it)->CreateInspectorNode();
						ImGui::TreePop();
					}

					ImGui::PopStyleColor();
				}

				ImGui::NewLine();
				//ImGui::Separator();

				i++;
			}

			static ImGuiComboFlags flags = 0;

			const char* items[] = { "Default", "Mesh", "Mesh Renderer" };
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

			// MYTODO: Allow dropping materials (big invisible button)

			//// --- Handle drag & drop ---
			//if (ImGui::BeginDragDropTarget())
			//{
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			//	{
			//		uint UID = *(const uint*)payload->Data;
			//		Resource* resource = App->resources->GetResource(UID, false);

			//		// MYTODO: Instance resource here, put it on scene (depending on resource)
			//		if (resource && resource->GetType() == Resource::ResourceType::SCRIPT)
			//		{
			//			resource = App->resources->GetResource(UID);
			//		}
			//	}

			//	ImGui::EndDragDropTarget();
			//}

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

			item_current = items[0];

			if (Startup)
				Startup = false;
		}
		// --- Display Resource Information ---
		else if (SelectedRes)
		{
			if(ImGui::BeginChild("res", ImVec2(0, 35), true))
				ImGui::Text(SelectedRes->GetName());

			ImGui::EndChild();

			if (ImGui::BeginChild("resdata", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), false))		
				SelectedRes->CreateInspectorNode();

			ImGui::EndChild();

		}
	}

	ImGui::End();

	ImGui::PopStyleVar();

	return true;
}

void PanelInspector::CreateGameObjectNode(GameObject& Selected) const
{

	if (ImGui::Checkbox("##GOActive", &Selected.GetActive()))
	{
		if (Selected.GetActive())
			Selected.Enable();
		else
			Selected.Disable();
	}

	ImGui::SameLine();

	// --- Game Object Name Setter ---
	static char GOName[100] = "";
	strcpy_s(GOName, 100, Selected.GetName().data());
	if (ImGui::InputText("", GOName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		Selected.SetName(GOName);

	ImGui::SameLine();

	if (ImGui::Checkbox("Static", &Selected.Static))
		App->scene_manager->SetStatic(&Selected);

}
