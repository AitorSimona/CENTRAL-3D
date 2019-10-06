#include "PanelScene.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "ModuleSceneIntro.h"


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
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		App->scene_intro->Draw();
	}

	ImGui::End();


	return true;
}
