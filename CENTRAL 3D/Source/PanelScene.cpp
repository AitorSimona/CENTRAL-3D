#include "PanelScene.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"

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
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("DebugDraw"))
			{
				ImGui::MenuItem("LIGHTING", NULL, &App->renderer3D->lighting);
				ImGui::MenuItem("COLOR MATERIAL", NULL, &App->renderer3D->color_material);
				ImGui::MenuItem("WIREFRAME", NULL, &App->renderer3D->wireframe);
		
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		App->scene_intro->Draw();
	}

	ImGui::End();


	return true;
}
