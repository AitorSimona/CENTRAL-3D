#include "PanelGame.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"

PanelGame::PanelGame(char* name) : Panel(name) 
{
	enabled = true;
}

PanelGame::~PanelGame() 
{
}

bool PanelGame::Draw() 
{

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		width = ImGui::GetWindowWidth();
		height = ImGui::GetWindowHeight();
		ImVec2 size = ImVec2(width, height);
		ImGui::Image((ImTextureID)App->renderer3D->rendertexture, size, ImVec2(0, 1), ImVec2(1, 0));

		SceneHovered = ImGui::IsWindowHovered();

		App->gui->sceneHeight = height;
		App->gui->sceneWidth = width;
		App->gui->sceneX = posX;
		App->gui->sceneY = posY;
		App->gui->hoveringScene = SceneHovered;

		ImGui::End();
	}
	return true;
}
