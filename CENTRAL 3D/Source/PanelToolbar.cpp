#include "PanelToolbar.h"
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
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

		if(ImGui::Button("PLAY"))
		{

		}
		ImGui::SameLine();

		if (ImGui::Button("PAUSE"))
		{

		}
		ImGui::SameLine();

		if (ImGui::Button("STOP"))
		{

		}
	}

	ImGui::End();


	return true;
}
