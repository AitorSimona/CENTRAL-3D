#include "PanelProject.h"

#include "Imgui/imgui.h"


#include "mmgr/mmgr.h"

PanelProject::PanelProject(char * name) : Panel(name)
{
}

PanelProject::~PanelProject()
{
}

bool PanelProject::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{

	}

	ImGui::End();


	return true;
}
