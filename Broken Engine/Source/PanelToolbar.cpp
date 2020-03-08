#include "PanelToolbar.h"
#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "PanelScene.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

PanelToolbar::PanelToolbar(char * name) : Broken::Panel(name)
{
	play_button = "PLAY";
	pause_button = "PAUSE";
}

PanelToolbar::~PanelToolbar()
{
}

bool PanelToolbar::Draw()
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::Button("TRANSLATE"))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		ImGui::SameLine();

		if (ImGui::Button("ROTATE"))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::ROTATE;
		ImGui::SameLine();

		if (ImGui::Button("SCALE"))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::SCALE;
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

		Broken::AppState app_status = EngineApp->GetAppState();

		if(ImGui::Button(play_button.c_str()))
		{
			if (app_status == Broken::AppState::PLAY || app_status == Broken::AppState::PAUSE) {
				EngineApp->GetAppState() = Broken::AppState::TO_EDITOR;
				play_button = "PLAY";
				pause_button = "PAUSE";
			}
			else {
				EngineApp->GetAppState() = Broken::AppState::TO_PLAY;
				play_button = "STOP";
			}
		}
		ImGui::SameLine();

		if (app_status != Broken::AppState::EDITOR && app_status != Broken::AppState::TO_EDITOR) {
			if (ImGui::Button(pause_button.c_str())) {
				if (app_status == Broken::AppState::PLAY) {
					EngineApp->GetAppState() = Broken::AppState::TO_PAUSE;
					pause_button = "PLAY";
				}
				else if (app_status == Broken::AppState::PAUSE) {
					EngineApp->GetAppState() = Broken::AppState::TO_PLAY;
					pause_button = "PAUSE";
				}

			}
			ImGui::SameLine();

			if (app_status == Broken::AppState::PAUSE) {
				if (ImGui::Button("STEP")) {
					if (app_status == Broken::AppState::PAUSE)
						EngineApp->GetAppState() = Broken::AppState::STEP;

				}

				ImGui::SameLine();
			}
		}
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.05f);

		float scale = EngineApp->time->GetTimeScale();

		ImGui::DragFloat("Time Scale", &scale, 0.005f);


		if(scale != EngineApp->time->GetTimeScale())
			EngineApp->time->SetTimeScale(scale);
	}
	ImGui::End();




	return true;
}
