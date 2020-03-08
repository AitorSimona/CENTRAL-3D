#include "ModuleGameManager.h"
#include "GameApplication.h"

ModuleGameManager::ModuleGameManager(bool start_enabled) {
	name = "Game Manager";
}

ModuleGameManager::~ModuleGameManager() {
}

bool ModuleGameManager::Init(Broken::json& config) {
	gameApp->scene_manager->display_grid = false;
	gameApp->renderer3D->renderfbo = false;
	gameApp->scene_manager->LoadGame(config);

	return true;
}

bool ModuleGameManager::Start() {
	gameApp->gui->sceneHeight = gameApp->window->GetWindowHeight();
	gameApp->gui->sceneWidth = gameApp->window->GetWindowWidth();
	return true;
}

update_status ModuleGameManager::PreUpdate(float dt) {
	
	//We just keep updating this as a workaround
	gameApp->gui->sceneHeight = gameApp->window->GetWindowHeight();
	gameApp->gui->sceneWidth = gameApp->window->GetWindowWidth();
	gameApp->gui->isSceneHovered = gameApp->window->isMouseFocused();

	return UPDATE_CONTINUE;
}

update_status ModuleGameManager::PostUpdate(float dt) {
	if (gameApp->GetAppState() == Broken::AppState::EDITOR)
		gameApp->GetAppState() = Broken::AppState::TO_PLAY;
	return UPDATE_CONTINUE;
}
