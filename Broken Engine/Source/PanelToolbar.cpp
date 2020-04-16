#include "PanelToolbar.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "ModuleGui.h"
#include "ModuleTimeManager.h"

// -- Panels --
#include "PanelScene.h"
#include "PanelHierarchy.h"

// -- Utilitites --
#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

PanelToolbar::PanelToolbar(char * name) : Panel(name)
{
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
		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->translateTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		ImGui::SameLine();
		ImGui::SetCursorPosX(38);

		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->rotateTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::ROTATE;
		ImGui::SameLine();
		ImGui::SetCursorPosX(68);

		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->scaleTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
			EngineApp->editorui->panelScene->guizmoOperation = ImGuizmo::OPERATION::SCALE;
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

		Broken::AppState app_status = EngineApp->GetAppState();

		// -- Play button
		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->toolbarPlayTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (app_status == Broken::AppState::EDITOR)
			{
				EngineApp->GetAppState() = Broken::AppState::TO_PLAY;

				if (EngineApp->gui->editingPrefab)
					EngineApp->editorui->panelHierarchy->ExitEditPrefab();
			}

			if (app_status == Broken::AppState::PLAY || app_status == Broken::AppState::PAUSE)
			{
				EngineApp->GetAppState() = Broken::AppState::TO_EDITOR;
			}
			
		}
		ImGui::SameLine();
		
		// -- Pause button
		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->toolbarPauseTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (app_status == Broken::AppState::PLAY)
				EngineApp->GetAppState() = Broken::AppState::TO_PAUSE;

			if (app_status == Broken::AppState::PAUSE)
				EngineApp->GetAppState() = Broken::AppState::TO_PLAY;
		}
		
		ImGui::SameLine();

		// -- Step button
		if (ImGui::ImageButton((ImTextureID)EngineApp->gui->toolbarStepTexID, ImVec2(28, 28), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (app_status == Broken::AppState::PAUSE)
			{
				EngineApp->GetAppState() = Broken::AppState::STEP;
			}
		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.05f);

		float scale = EngineApp->time->GetTimeScale();

		ImGui::DragFloat("Time Scale", &scale, 0.005f);

		if(scale != EngineApp->time->GetTimeScale())
			EngineApp->time->SetTimeScale(scale);

		if (app_status == Broken::AppState::PLAY || app_status == Broken::AppState::PAUSE || app_status == Broken::AppState::STEP)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.7, 1.0, 1.0, 1.0), "Elapsed time: %.5f", EngineApp->time->GetGameplayTimePassed());
		}
		

	}
	ImGui::End();




	return true;
}
