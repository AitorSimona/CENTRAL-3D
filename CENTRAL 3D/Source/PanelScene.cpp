#include "PanelScene.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleSceneManager.h"

#include "ModuleImporter.h"
#include "ImporterScene.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"

#include "PanelProject.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

PanelScene::PanelScene(char * name) : Panel(name)
{
}

PanelScene::~PanelScene()
{
}


bool PanelScene::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		ImVec2 size = ImVec2(ImGui::GetWindowWidth()*0.987, ImGui::GetWindowHeight()*0.93);
	
		ImGui::Image((ImTextureID)App->renderer3D->rendertexture, size, ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX"))
			{
				if (App->gui->panelProject)
				{
					std::string extension;
					App->fs->SplitFilePath(App->gui->panelProject->dragged.data(), nullptr, nullptr, &extension);

					if (extension.compare("fbx") == 0 || extension.compare("FBX") == 0)
					{
						ImportData data;
						App->importer->GetImporterScene()->Import(App->gui->panelProject->dragged.data(), data);
						App->gui->panelProject->dragged = "";
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		// --- Process input and update editor camera ---
		if (ImGui::IsWindowHovered())
			SceneHovered = true;
		else
			SceneHovered = false;


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("DebugDraw"))
			{
				ImGui::MenuItem("LIGHTING", NULL, &App->renderer3D->lighting);
				ImGui::MenuItem("COLOR MATERIAL", NULL, &App->renderer3D->color_material);
				ImGui::MenuItem("WIREFRAME", NULL, &App->renderer3D->wireframe);
				ImGui::MenuItem("BOUNDING BOXES", NULL, &App->scene_manager->display_boundingboxes);
				ImGui::MenuItem("OCTREE", NULL, &App->scene_manager->display_tree);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}

	ImGui::End();


	return true;
}

void PanelScene::HandleInput()
{

}
