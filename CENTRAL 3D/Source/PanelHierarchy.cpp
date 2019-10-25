#include "PanelHierarchy.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"


#include "mmgr/mmgr.h"

PanelHierarchy::PanelHierarchy(char * name) : Panel(name)
{
}

PanelHierarchy::~PanelHierarchy()
{
}

bool PanelHierarchy::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		ImGui::BeginChild("GO_List", ImVec2(325, 0), true);
		for (uint i = 0; i < App->scene_manager->GetNumGameObjects(); i++)
		{
			if (ImGui::Selectable(App->scene_manager->GetGameObjects().at(i)->GetName().data(), App->scene_manager->GetSelectedGameObjects() == i))
				App->scene_manager->SetSelectedGameObject(i);
		}
		ImGui::EndChild();
		ImGui::SameLine();
	}

	ImGui::End();


	return true;
}
