#include "ScriptingScenes.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "ImporterModel.h"
#include "ScriptData.h"
#include "ResourceScene.h"
#include "ResourceModel.h"
#include "ComponentTransform.h"

using namespace Broken;
ScriptingScenes::ScriptingScenes() {}

ScriptingScenes::~ScriptingScenes() {}

void ScriptingScenes::LoadSceneFromScript(uint scene_UUID)
{
	ResourceScene* scene = (ResourceScene*)App->resources->GetResource(scene_UUID, false);
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

uint ScriptingScenes::Instantiate(uint resource_UUID, float x, float y, float z, float alpha, float beta, float gamma)
{
	ResourceModel* model = (ResourceModel*)App->resources->GetResource(resource_UUID);
	GameObject* go = App->resources->GetImporter<ImporterModel>()->InstanceOnCurrentScene(model->GetResourceFile(), nullptr);
	go->GetComponent<ComponentTransform>()->SetPosition(x, y, z);
	go->GetComponent<ComponentTransform>()->SetRotation({ alpha, beta, gamma });

	return go->GetUID();
}