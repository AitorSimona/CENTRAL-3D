#include "PanelHierarchy.h"
#include "Imgui/imgui.h"

#include "EngineApplication.h"
//#include "ModuleSceneManager.h"
//#include "ModuleInput.h"
//#include "ImporterScene.h"
//#include "ModuleFileSystem.h"

//#include "GameObject.h"
#include "PanelProject.h"

//
//#include "mmgr/mmgr.h"

PanelHierarchy::PanelHierarchy(char * name) : BrokenEngine::Panel(name)
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
		DrawRecursive(EngineApp->scene_manager->GetRootGO());
	}

	ImGui::End();

	// --- Manage Drag & Drop ---
	if (end_drag)
	{
		if (!dragged->FindChildGO(target) && target != dragged)
			target->AddChildGO(dragged);

		end_drag = false;
		dragged = nullptr;
		target = nullptr;
	}
	if (to_destroy)
	{
		EngineApp->scene_manager->DestroyGameObject(to_destroy);
		to_destroy = nullptr;
		EngineApp->scene_manager->SetSelectedGameObject(nullptr);
	}

	return true;
}

void PanelHierarchy::DrawRecursive(BrokenEngine::GameObject * Go)
{
	// --- Set node flags ---
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (Go == EngineApp->scene_manager->GetSelectedGameObject())
		node_flags |= ImGuiTreeNodeFlags_Selected;

	// --- Avoid displaying root ---
	if (Go->GetName().c_str() == EngineApp->scene_manager->GetRootGO()->GetName())
	{
		if (Go->childs.size() > 0)
		{
			for (std::vector<BrokenEngine::GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
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

		if(!Go->GetActive())
		ImGui::PushStyleColor(ImGuiCol(), ImVec4(0.5, 0.5, 0.5, 1));

		ImGui::Image((ImTextureID)EngineApp->gui->prefabTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();

		bool open = ImGui::TreeNodeEx((void*)Go->GetUID(), node_flags, Go->GetName().c_str());

		if (!Go->GetActive())
		ImGui::PopStyleColor();

		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("GO", Go, sizeof(BrokenEngine::GameObject));        // Set payload to carry the index of our item (could be anything)
			dragged = Go;
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GO"))
			{
				target = Go;
				end_drag = true;
			}

			
			ImGui::EndDragDropTarget();
		}

		// --- Set Game Object to be destroyed ---
		if (ImGui::IsWindowFocused() && Go == EngineApp->scene_manager->GetSelectedGameObject() && EngineApp->input->GetKey(SDL_SCANCODE_DELETE) == BrokenEngine::KEY_DOWN)
		{
			EX_ENGINE_CONSOLE_LOG("Destroying: %s ...", Go->GetName().c_str());
			to_destroy = Go;
		}

		// --- If node is clicked set Go as selected ---
		if (ImGui::IsItemClicked())
			EngineApp->scene_manager->SetSelectedGameObject(Go);


		// --- Display children only if current node is open ---
		if (open)
		{
			// --- Check for children and draw them the same way ---
			if (Go->childs.size() > 0)
			{
				for (std::vector<BrokenEngine::GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
				{
					DrawRecursive(*it);
				}
			}

			ImGui::TreePop();
		}

	}
}
