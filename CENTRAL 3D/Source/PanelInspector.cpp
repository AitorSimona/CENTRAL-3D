#include "PanelInspector.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"

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
		GameObject* Selected = App->scene_manager->GetGameObjects().at(App->scene_manager->GetSelectedGameObjects());

		ImGui::SetNextItemOpen(true);

		if (ImGui::TreeNode("Transform"))
		{

			// --- Transform ---
			float3 position = Selected->GetPosition();
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);
	
			ImGui::DragFloat(" ", &position.x, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			ImGui::DragFloat("  ", &position.y, 0.005f);
				
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			ImGui::DragFloat("   ", &position.z, 0.005f);

			Selected->SetPosition(position.x, position.y, position.z);

			ImGui::TreePop();
		}

		ImGui::Separator();


		if (Selected->GetComponent(Component::ComponentType::Mesh))
		{

		}
	}

	ImGui::End();


	return true;
}
