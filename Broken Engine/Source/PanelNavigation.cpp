#include "PanelNavigation.h"
#include "Imgui/imgui.h"
#include "EngineApplication.h"
#include "ModuleRecast.h"

PanelNavigation::PanelNavigation(char* name) : Panel(name) {
}

PanelNavigation::~PanelNavigation() {
}

bool PanelNavigation::Draw() {
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		EngineApp->detour->setDebugDraw(true);
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
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi0", EngineApp->detour->areaNames[0], 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				float areaCost = EngineApp->detour->areaCosts[0];
				if (ImGui::InputFloat("##cost_bi0", &areaCost))
					EngineApp->detour->setAreaCost(0, areaCost);

				ImGui::NextColumn();
				ImGui::Text("Built-in 1");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi1", EngineApp->detour->areaNames[1], 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				char buf[10];
				sprintf_s(buf, "1");
				ImGui::InputText("##cost_bi1", buf, 10, ImGuiInputTextFlags_ReadOnly);

				ImGui::NextColumn();
				ImGui::Text("Built-in 2");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				ImGui::InputText("##name_bi2", EngineApp->detour->areaNames[2], 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
				areaCost = EngineApp->detour->areaCosts[2];
				if (ImGui::InputFloat("##cost_bi2", &areaCost))
					EngineApp->detour->setAreaCost(2, areaCost);

				std::string catname = "User ";
				std::string namelabel = "##name_us";
				std::string costlabel = "##cost_us";

				for (int i = 3; i < BE_DETOUR_TOTAL_AREAS; ++i) {
					ImGui::NextColumn();
					ImGui::Text((catname + std::to_string(i)).c_str());
					ImGui::NextColumn();
					ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
					ImGui::InputText((namelabel + std::to_string(i)).c_str(), EngineApp->detour->areaNames[i], 100);
					ImGui::NextColumn();
					ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - 10);
					areaCost = EngineApp->detour->areaCosts[i];
					if (ImGui::InputFloat((costlabel + std::to_string(i)).c_str(), &areaCost))
						EngineApp->detour->setAreaCost(i, areaCost);
				}



				ImGui::Columns(1);
			
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Bake")) {
				ImGui::Text("Baked Agent Size");
				bool updateValues;
				if (ImGui::DragFloat("Agent Radius", &EngineApp->detour->agentRadius, 0.25f, 0.01f, 9999999999.0f, "%.2f", 1.0f)) {
				}
				if (ImGui::DragFloat("Agent Height", &EngineApp->detour->agentHeight, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f)) {
				}
				if (ImGui::SliderFloat("Max Slope", &EngineApp->detour->maxSlope, 0.0f, 60.0f, "%.1f")) {
				}
				if (ImGui::DragFloat("Step Height", &EngineApp->detour->stepHeight, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f)) {
				}

				if (ImGui::TreeNode("Advanced")) {
					ImGui::Text("Manual Voxel Size"); ImGui::SameLine();
					ImGui::Checkbox("##manualVoxelSize", &manualVoxel);
					if (manualVoxel)
						ImGui::DragFloat("Voxel Size##dragFloat", &EngineApp->detour->voxelSize, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					else {
						EngineApp->detour->voxelSize = EngineApp->detour->agentRadius / 3.0f;
						float voxelSize = EngineApp->detour->voxelSize;
						ImGui::DragFloat("Voxel Size##inputText", &voxelSize, 0.0f, 0.0f, 0.0f, "%.2f");
					}

					ImGui::DragFloat("Voxel Height", &EngineApp->detour->voxelHeight, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);

					ImGui::DragFloat("Region Minimum Size", &EngineApp->detour->regionMinSize, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Region Merge Size", &EngineApp->detour->regionMergeSize, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Edge Maximum Length", &EngineApp->detour->edgeMaxLen, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Edge Maximum Error", &EngineApp->detour->edgeMaxError, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Vertices Per Polygon", &EngineApp->detour->vertsPerPoly, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Detail Sample Distance", &EngineApp->detour->detailSampleDist, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::DragFloat("Detail Sample Max. Error", &EngineApp->detour->detailSampleMaxError, 0.25f, 0.02f, 9999999999.0f, "%.2f", 1.0f);
					ImGui::Text("Build Tiled Mesh"); ImGui::SameLine();
					ImGui::Checkbox("##tiledMesh", &EngineApp->detour->buildTiledMesh);
					ImGui::TreePop();
				}
				ImGui::Separator();
				if (EngineApp->detour->getNavMeshResource() != nullptr) {
					if (ImGui::Button("Clear")) {
						EngineApp->detour->deleteNavMesh();
					}
					ImGui::SameLine();
				}
				if (ImGui::Button("Bake")) {
					EngineApp->recast->BuildNavMesh();
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Object")) {
				//MYTODO this should probably be a MeshRenderer rather than a mesh, but Cameras GO have MeshRenderers
				uint selected_GOs = EngineApp->selection->GetSelected()->size();
				if (selected_GOs <= 1) {
					Broken::GameObject* selected = EngineApp->selection->GetLastSelected();
					if (selected != nullptr && selected->GetComponent<Broken::ComponentMesh>() != nullptr) {
						ImGui::Text(selected->GetName());
						ImGui::Separator();
						ImGui::Text("Navigation Static"); ImGui::SameLine();
						if (ImGui::BeginPopup("Change Static Flags")) {
							std::string enable;
							if (popupNavigationFlag)
								enable = "enable";
							else
								enable = "disable";

							ImGui::Text(std::string("Do you want to " + enable + " the Navigation Static flag for al the child objects as well?").c_str());
							if (ImGui::Button("Yes, change children")) {
								std::queue<Broken::GameObject*> childs;
								childs.push(selected);

								while (!childs.empty()) {
									Broken::GameObject* current_child = childs.front();
									childs.pop();

									//We add all of its childs
									for (uint i = 0; i < current_child->childs.size(); i++)
										childs.push(current_child->childs[i]);

									//We change the value of static
									if (current_child->GetComponent<Broken::ComponentMesh>() != nullptr && current_child->navigationStatic != popupNavigationFlag) {
										current_child->navigationStatic = popupNavigationFlag;
										if (popupNavigationFlag)
											EngineApp->recast->AddGO(current_child);
										else
											EngineApp->recast->DeleteGO(current_child);
									}
								}
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button("No, this object only")) {
								selected->navigationStatic = popupNavigationFlag;
								if (popupNavigationFlag)
									EngineApp->recast->AddGO(selected);
								else
									EngineApp->recast->DeleteGO(selected);
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button("Cancel"))
								ImGui::CloseCurrentPopup();

							ImGui::EndPopup();
						}
						navigationStatic = selected->navigationStatic;
						if (ImGui::Checkbox("##navigationStaticCheckbox", &navigationStatic)) {
							if (selected->childs.size() > 0) {
								popupNavigationFlag = navigationStatic;
								ImGui::OpenPopup("Change Static Flags");
							}
							else {
								selected->navigationStatic = navigationStatic;
								if (navigationStatic)
									EngineApp->recast->AddGO(selected);
								else
									EngineApp->recast->DeleteGO(selected);
							}

						}

						if (selected->navigationStatic) {

							if (ImGui::BeginPopup("Change Navigation Area")) {
								std::string popupAreaName = EngineApp->detour->areaNames[popupArea];
								ImGui::Text(std::string("Do you want to change the navigation area to " + popupAreaName + " for all the child objects as well?").c_str());
								if (ImGui::Button("Yes, change children")) {
									std::queue<Broken::GameObject*> childs;
									childs.push(selected);

									while (!childs.empty()) {
										Broken::GameObject* current_child = childs.front();
										childs.pop();

										//We add all of its childs
										for (uint i = 0; i < current_child->childs.size(); i++)
											childs.push(current_child->childs[i]);

										//We change the value of static
										if (current_child->GetComponent<Broken::ComponentMesh>() != nullptr)
											current_child->navigationArea = popupArea;
									}
									ImGui::CloseCurrentPopup();
								}
								ImGui::SameLine();
								if (ImGui::Button("No, this object only")) {
									selected->navigationArea = popupArea;
									ImGui::CloseCurrentPopup();
								}
								ImGui::SameLine();
								if (ImGui::Button("Cancel"))
									ImGui::CloseCurrentPopup();

								ImGui::EndPopup();
							}

							ImGui::Text("Navigation Area"); ImGui::SameLine();
							if (ImGui::BeginCombo("##areaCombo", EngineApp->detour->areaNames[selected->navigationArea])) {
								for (int i = 0; i < BE_DETOUR_TOTAL_AREAS; ++i) {
									std::string areaName = EngineApp->detour->areaNames[i];
									if (areaName != "") {
										ImGui::PushID((void*)EngineApp->detour->areaNames[i]);
										if (ImGui::Selectable(EngineApp->detour->areaNames[i], i == selected->navigationArea)) {
											if (selected->childs.size() != 0) {
												popupArea = i;
												openPopup = true;
											}
											else
												selected->navigationArea = i;
										}
										ImGui::PopID();
									}
								}
								ImGui::EndCombo();
							}
							if (openPopup) {
								ImGui::OpenPopup("Change Navigation Area");
								openPopup = false;
							}
						}

					}
					else
						ImGui::Text("Select one mesh from the scene.");
				}
				else {
					ImGui::Text("You selected multiple GOs!");
					ImGui::Text("Select just one mesh from the scene.");
				}
				
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	else {
		EngineApp->detour->setDebugDraw(false);
	}
	ImGui::End();
	return true;
}

void PanelNavigation::SetOnOff(bool status) {
	enabled = status;
	if (!status)
		EngineApp->detour->setDebugDraw(false);
}

void PanelNavigation::OnOff() {
	enabled = !enabled;
	if (!enabled)
		EngineApp->detour->setDebugDraw(false);
}
