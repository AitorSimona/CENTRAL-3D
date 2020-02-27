#include "PanelToolbar.h"
#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "PanelScene.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

PanelToolbar::PanelToolbar(char * name) : BrokenEngine::Panel(name)
{
}

PanelToolbar::~PanelToolbar()
{
}

bool PanelToolbar::Draw()
{
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

		if(ImGui::Button("PLAY"))
		{
			if (EngineApp->GetAppState() == BrokenEngine::AppState::PLAY || EngineApp->GetAppState() == BrokenEngine::AppState::PAUSE)
				EngineApp->GetAppState() = BrokenEngine::AppState::TO_EDITOR;
			else
				EngineApp->GetAppState() = BrokenEngine::AppState::TO_PLAY;
		}
		ImGui::SameLine();

		if (ImGui::Button("PAUSE"))
		{
			if (EngineApp->GetAppState() == BrokenEngine::AppState::PLAY)
				EngineApp->GetAppState() = BrokenEngine::AppState::TO_PAUSE;
			else if (EngineApp->GetAppState() == BrokenEngine::AppState::PAUSE)
				EngineApp->GetAppState() = BrokenEngine::AppState::TO_PLAY;

		}
		ImGui::SameLine();

		if (ImGui::Button("STEP"))
		{
			if (EngineApp->GetAppState() == BrokenEngine::AppState::PAUSE)
				EngineApp->GetAppState() = BrokenEngine::AppState::STEP;

		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.05f);

		float scale = EngineApp->time->GetTimeScale();

		ImGui::DragFloat("Time Scale", &scale, 0.005f);


		if(scale != EngineApp->time->GetTimeScale())
			EngineApp->time->SetTimeScale(scale);
	}

	ImGui::End();


	return true;
}
