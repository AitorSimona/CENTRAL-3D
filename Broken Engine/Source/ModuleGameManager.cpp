#include "ModuleGameManager.h"
#include "GameApplication.h"

ModuleGameManager::ModuleGameManager(bool start_enabled) {
	name = "Game Manager";
}

ModuleGameManager::~ModuleGameManager() {
}

bool ModuleGameManager::Init(Broken::json& config) {
	GameApp->renderer3D->display_grid = false;
	GameApp->renderer3D->renderfbo = true;
	GameApp->renderer3D->drawfb = true;

	return true;
}

bool ModuleGameManager::Start() {
	GameApp->gui->sceneHeight = GameApp->window->GetWindowHeight();
	GameApp->gui->sceneWidth = GameApp->window->GetWindowWidth();
	GameApp->scene_manager->LoadGame(GameApp->GetConfigFile());
	return true;
}

update_status ModuleGameManager::PreUpdate(float dt) {
	
	//We just keep updating this as a workaround
	GameApp->gui->sceneHeight = GameApp->window->GetWindowHeight();
	GameApp->gui->sceneWidth = GameApp->window->GetWindowWidth();
	GameApp->gui->isSceneHovered = GameApp->window->isMouseFocused();

	return UPDATE_CONTINUE;
}

update_status ModuleGameManager::PostUpdate(float dt) {
	if (GameApp->GetAppState() == Broken::AppState::EDITOR)
		GameApp->GetAppState() = Broken::AppState::TO_PLAY;
	return UPDATE_CONTINUE;
}
