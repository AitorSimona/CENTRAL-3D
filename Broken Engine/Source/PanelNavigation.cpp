#include "PanelNavigation.h"
#include "Imgui/imgui.h"
#include "EngineApplication.h"

PanelNavigation::PanelNavigation(char* name) : Panel(name) {
}

PanelNavigation::~PanelNavigation() {
}

bool PanelNavigation::Draw() {
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("NavigationTabBar", tab_bar_flags)) {
			if (ImGui::BeginTabItem("Areas")) {
				ImGui::Columns(3, "mycolumns3", false);
				ImGui::SetColumnWidth(0,100);
				ImGui::SetColumnWidth(2, 100);
				ImGui::SetColumnOffset(1, 105);
				float middleColumn = math::Max<float>(ImGui::GetWindowWidth() - 210, 100);
				ImGui::SetColumnWidth(1, middleColumn);
				ImGui::SetColumnOffset(2, 105 + middleColumn);

				//Indeces
				ImGui::NextColumn();
				ImGui::Text("Name");
				ImGui::NextColumn();
				ImGui::Text("Cost");
				ImGui::Separator();

				//Built-ins
				ImGui::NextColumn();
				ImGui::Text("Built-in 0");
				ImGui::NextColumn();
				char buf[100];
				sprintf_s(buf, "Walkable");
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi0", buf, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				int cost = 1;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputInt("##cost_bi0", &cost);

				ImGui::NextColumn();
				ImGui::Text("Built-in 1");
				ImGui::NextColumn();
				sprintf_s(buf, "Not Walkable");
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi1", buf, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				sprintf_s(buf, "1");
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##cost_bi1", buf, ImGuiInputTextFlags_ReadOnly);

				ImGui::NextColumn();
				ImGui::Text("Built-in 2");
				ImGui::NextColumn();
				sprintf_s(buf, "Jump");
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi2", buf, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				int jumpcost = 2;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputInt("##cost_bi2", &jumpcost);



				ImGui::Columns(1);
			
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Bake")) {
				ImGui::Text("Baked Agent Size");
				if (ImGui::DragFloat("Agent Radius", &agentRadius, 1.0f, 0.01f, 0.0f, "%.2f", 1.0f)) {
				}
				if (ImGui::DragFloat("Agent Height", &agentHeight, 1.0f, 0.02f, 0.0f, "%.2f", 1.0f)) {
				}
				if (ImGui::SliderFloat("Max Slope", &maxSlope, 0.0f, 60.0f, "%.1f")) {
				}
				if (ImGui::DragFloat("Step Height", &stepHeight, 1.0f, 0.02f, 0.0f, "%.2f", 1.0f)) {
				}

				if (ImGui::Button("Clear")) {
					//MYTODO delete the navmesh binary
				}
				ImGui::SameLine();
				if (ImGui::Button("Build")) {
					//MYTODO create the NavMesh binary
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Object")) {
				//MYTODO this should probably be a MeshRenderer rather than a mesh, but Cameras GO have MeshRenderers
				Broken::GameObject* selected = EngineApp->scene_manager->GetSelectedGameObject();
				if (selected != nullptr &&  selected->GetComponent<Broken::ComponentMesh>() != nullptr) {
					ImGui::Text(selected->GetName());
					ImGui::Separator();
					ImGui::Text("Navigation Static"); ImGui::SameLine();
					if (ImGui::Checkbox("##navigationStaticCheckbox", &selected->navigationStatic)) {
						//MYTODO Add it to the list of our own module
					}

					if (selected->navigationStatic) 						{
						const char* items[] = { "Walkable", "Not Walkable", "Jump" };
						static int item = -1;
						ImGui::Text("Navigation Area"); ImGui::SameLine();
						ImGui::Combo("##areaCombo", &item, items, IM_ARRAYSIZE(items));


					}

				}
				else
					ImGui::Text("Select a mesh from the scene.");
				
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
	return true;
}