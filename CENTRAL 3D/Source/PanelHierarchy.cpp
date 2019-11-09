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
		DrawRecursive(App->scene_manager->GetRootGO());
	}

	ImGui::End();

	return true;
}

void PanelHierarchy::DrawRecursive(GameObject * Go)
{
	// --- Set node flags ---
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (Go == App->scene_manager->GetSelectedGameObjects())
		node_flags |= ImGuiTreeNodeFlags_Selected;

	// --- Avoid displaying root ---
	if (Go->GetName().data() == App->scene_manager->GetRootGO()->GetName())
	{
		if (Go->childs.size() > 0)
		{
			for (std::vector<GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
			{
				DrawRecursive(*it);
			}
		}
	}

	// --- Display Go node ---
    else 
	{
		// --- Create current node and get if it is opened or not ---
		bool open = ImGui::TreeNodeEx(Go->GetName().data(), node_flags);

		// --- If node is clicked set Go as selected ---
		if (ImGui::IsItemClicked())
			App->scene_manager->SetSelectedGameObject(Go);

		// --- Displat children only if current node is open ---
		if (open)
		{
			// --- Check for children and draw them the same way ---
			if (Go->childs.size() > 0)
			{
				for (std::vector<GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
				{
					DrawRecursive(*it);
				}
			}

			ImGui::TreePop();
		}

	}
}
