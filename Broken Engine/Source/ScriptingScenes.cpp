#include "ScriptingScenes.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingScenes::ScriptingScenes() {}

ScriptingScenes::~ScriptingScenes() {}

void ScriptingScenes::LoadSceneFromScript(uint UUID)
{
	ResourceScene* scene = (ResourceScene*)App->resources->GetResource(UUID);
	App->scene_manager->SetActiveScene(scene);
}

void ScriptingScenes::QuitGame()
{
	/*if (App->isGame)

	else 
		App->GetAppState() = Broken::AppState::TO_EDITOR;*/
}