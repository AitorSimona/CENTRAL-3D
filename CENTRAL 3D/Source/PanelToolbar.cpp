#include "PanelToolbar.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleTimeManager.h"

#include "PanelHierarchy.h"
#include "PanelScene.h"

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
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::RadioButton("World", App->gui->panelScene->guizmoMode == ImGuizmo::WORLD))
			App->gui->panelScene->guizmoMode = ImGuizmo::WORLD;

		ImGui::SameLine();

		if (ImGui::RadioButton("Local", App->gui->panelScene->guizmoMode == ImGuizmo::LOCAL))
			App->gui->panelScene->guizmoMode = ImGuizmo::LOCAL;

		ImGui::SameLine();

		ImGui::Checkbox("Snap", &App->gui->panelScene->use_snap);

		ImGui::SameLine();

		ImGui::Checkbox("BoundSizing", &App->gui->panelScene->boundSizing);


		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);
		ImGui::SetCursorPosY(ImGui::GetWindowPos().y + 5);

		if(ImGui::Button("PLAY"))
		{
			if (App->GetAppState() == AppState::PLAY || App->GetAppState() == AppState::PAUSE)
				App->GetAppState() = AppState::TO_EDITOR;
			else
			{
				App->GetAppState() = AppState::TO_PLAY;

				if(App->gui->panelHierarchy->editingPrefab)
					App->gui->panelHierarchy->ExitEditPrefab();
			}
		}
		ImGui::SameLine();

		if (ImGui::Button("PAUSE"))
		{
			if (App->GetAppState() == AppState::PLAY)
				App->GetAppState() = AppState::TO_PAUSE;
			else if (App->GetAppState() == AppState::PAUSE)
				App->GetAppState() = AppState::TO_PLAY;

		}
		ImGui::SameLine();

		if (ImGui::Button("STEP"))
		{
			if (App->GetAppState() == AppState::PAUSE)
				App->GetAppState() = AppState::STEP;

		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth()*0.05f);

		float scale = App->time->GetTimeScale();

		ImGui::DragFloat("Time Scale", &scale, 0.005f);


		if(scale != App->time->GetTimeScale())
			App->time->SetTimeScale(scale);
	}

	ImGui::End();


	return true;
}
