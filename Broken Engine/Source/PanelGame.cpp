#include "PanelGame.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"

PanelGame::PanelGame(char* name) : Panel(name) 
{
	enabled = true;
}

PanelGame::~PanelGame() 
{
}

bool PanelGame::Draw() 
{
	App->gui->sceneHeight = App->window->GetWindowHeight();
	App->gui->sceneWidth = App->window->GetWindowWidth();

	SceneHovered = ImGui::IsWindowHovered();

	return true;
}
