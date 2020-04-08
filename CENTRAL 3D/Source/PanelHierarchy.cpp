#include "PanelHierarchy.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleSceneManager.h"
#include "ModuleInput.h"
#include "ImporterScene.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "PanelProject.h"

#include "ResourceScene.h"

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
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::Image((ImTextureID)App->gui->sceneTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
			ImGui::Text(App->scene_manager->currentScene->GetName());
		}
		ImGui::EndMenuBar();

		DrawRecursive(App->scene_manager->GetRootGO());
	}

	// Deselect the current GameObject when clicking in an empty space of the hierarchy
	if (ImGui::InvisibleButton("##Deselect", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - ImGui::GetCursorPosY() }))
		App->scene_manager->SetSelectedGameObject(nullptr);

	ImGui::End();

	return true;
}

void PanelHierarchy::DrawRecursive(GameObject* Go)
{
	// --- Set node flags ---
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (Go == App->scene_manager->GetSelectedGameObject())
		node_flags |= ImGuiTreeNodeFlags_Selected;

	// --- Avoid displaying root ---
	if (Go->GetName().c_str() == App->scene_manager->GetRootGO()->GetName())
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
		if (Go->childs.size() == 0)
			node_flags |= ImGuiTreeNodeFlags_Leaf;

		// --- Create current node and get if it is opened or not ---

		if (!Go->GetActive())
			ImGui::PushStyleColor(ImGuiCol(), ImVec4(0.5, 0.5, 0.5, 1));

		if (Go->is_prefab_child || Go->is_prefab_instance)
		{
			ImGui::Image((ImTextureID)App->gui->prefabTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::SameLine();
		}

		bool open = ImGui::TreeNodeEx((void*)Go->GetUID(), node_flags, Go->GetName().c_str());

		if (!Go->GetActive())
			ImGui::PopStyleColor();

		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			uint UID = Go->GetUID();
			ImGui::SetDragDropPayload("GO", &UID, sizeof(uint));
			ImGui::Text(Go->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GO"))
			{
				uint UID = *(const uint*)payload->Data;

				GameObject* go = App->scene_manager->currentScene->GetGOWithUID(UID);

				// --- Block move if go is prefab child ---
				if (go->is_prefab_child)
				{
					CONSOLE_LOG("Cannot move child of a Prefab instance %s ...", go->GetName().c_str());
				}
				else if (!go->FindChildGO(Go) && go->GetUID() != Go->GetUID())
					Go->AddChildGO(go);

			}
			ImGui::EndDragDropTarget();
		}

		// --- Set Game Object to be destroyed ---
		if (ImGui::IsWindowFocused() && Go == App->scene_manager->GetSelectedGameObject() && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			// --- Block delete if go is prefab child ---
			if (!Go->is_prefab_child)
			{
				CONSOLE_LOG("Destroying: %s ...", Go->GetName().c_str());
				App->scene_manager->SendToDelete(Go);
			}
			else
				CONSOLE_LOG("Cannot delete child of a Prefab instance %s ...", Go->GetName().c_str());

		}

		// --- If node is clicked set Go as selected ---
		if (ImGui::IsItemClicked())
			App->scene_manager->SetSelectedGameObject(Go);


		// --- Display children only if current node is open ---
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
