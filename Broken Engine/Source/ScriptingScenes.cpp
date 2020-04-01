#include "ScriptingScenes.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ScriptData.h"
#include "ResourceScene.h"

using namespace Broken;
ScriptingScenes::ScriptingScenes() {}

ScriptingScenes::~ScriptingScenes() {}

void ScriptingScenes::LoadSceneFromScript(uint UUID)
{
	ResourceScene* scene = (ResourceScene*)App->resources->GetResource(UUID, false);
	App->scene_manager->SetActiveScene(scene);
}

void ScriptingScenes::QuitGame()
{
	if (App->isGame)
		App->scripting->game_update = UPDATE_STOP;
	else 
	{
		App->GetAppState() = Broken::AppState::TO_EDITOR;
	}
}

//void ScriptingScenes::LoadSceneByName(const char* name)
//{
//	ResourceScene* scene = nullptr;
//	for (std::map<uint, Broken::ResourceScene*>::const_iterator it = App->resources->scenes.begin(); it != App->resources->scenes.end() && scene == nullptr; ++it) {
//		if ((*it).second->GetName() == name)
//			scene = (*it).second;
//
//	}
//
//	App->scene_manager->SetActiveScene(scene);
//
//}