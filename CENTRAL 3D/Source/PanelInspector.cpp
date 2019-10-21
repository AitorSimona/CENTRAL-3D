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

			// --- Transform Position ---
			ImGui::Text("Position  ");
			ImGui::SameLine();

			float3 position = Selected->GetPosition();
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);
	
			ImGui::DragFloat("PX", &position.x, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);

			ImGui::DragFloat("PY", &position.y, 0.005f);
				
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);

			ImGui::DragFloat("PZ", &position.z, 0.005f);

			// --- Transform Rotation ---
			/*float3 rotation = Selected->GetRotation();
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			ImGui::DragFloat("    ", &position.x, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			ImGui::DragFloat("     ", &position.y, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f);

			ImGui::DragFloat("      ", &position.z, 0.005f);

			Selected->SetPosition(position.x, position.y, position.z);*/

			// --- Transform Scale ---
			ImGui::Text("Scale     ");
			ImGui::SameLine();

			float3 scale = Selected->GetScale();
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);

			ImGui::DragFloat("SX", &scale.x, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);

			ImGui::DragFloat("SY", &scale.y, 0.005f);

			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);

			ImGui::DragFloat("SZ", &scale.z, 0.005f);


			// --- Transform Set ---
			Selected->SetPosition(position.x, position.y, position.z);
			Selected->Scale(scale.x, scale.y, scale.z);


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
