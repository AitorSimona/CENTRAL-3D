#include "PanelBuild.h"
#include "JSONLoader.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Globals.h"
#include "ModuleThreading.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"
#include <queue>
#include "Component.h"
#include "Imgui/imgui.h"

bool PanelBuild::Draw() {
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		ImGui::Separator();
		ImGui::Text("Choose build name");

		ImGui::End();
	}



	return true;
}

inline void PanelBuild::OnOff() {
	if (!enabled) {
		enabled = true;
		findCameras();
	}
	else
		enabled = false;
}

inline void PanelBuild::SetOnOff(bool set) {
	if (set)
		findCameras();
	enabled = set;
}

void PanelBuild::findCameras() {
	cameras.clear();
	std::queue<GameObject*> GOqueue;
	GOqueue.push(App->scene_manager->GetRootGO());

	while (!GOqueue.empty()) {
		GameObject* current = GOqueue.front();
		GOqueue.pop();

		Component* cameraComponent = current->HasComponent(Component::ComponentType::Camera);
		if (cameraComponent != nullptr)
			cameras.push_back(current);

		for (int i = 0; i < current->childs.size(); ++i)
			GOqueue.push(current->childs[i]);
	}
	
}

void PanelBuild::makeBuild() {

	// Create build folder
	App->fs->CreateDirectoryA(buildName.c_str());

	//We copy the executable
	App->fs->Copy(GAME_EXE, (buildName + "/" + buildName + ".exe").c_str());

	std::vector<std::string> files;
	std::vector<std::string> dirs;
	App->fs->DiscoverFiles("", files, dirs);

	//We copy all dlls and .ini
	for (int i = 0; i < files.size(); ++i) {
		std::string extension = (files[i].substr(files[i].find_last_of(".") + 1));
		App->fs->NormalizePath(extension);

		if (extension == "dll" || extension == "ini")
			App->fs->Copy(files[i].c_str(), (buildName + "/" + files[i]).c_str());
	}
	
	for (int i = 0; i < dirs.size(); ++i)
		App->threading->ADDTASK(this, PanelBuild::copyAllFolderMT, dirs[i].c_str());

	//We wait for the module threading to finish tasks
	App->threading->FinishProcessing();

	//We write our settings to gameSettings.
	std::string settingsPath = buildName + "/Settings/GameConfig.json";
	json gameSettings = App->GetJLoader()->Load(settingsPath.c_str());
	gameSettings["Application"]["Title"] = buildName.c_str();
	gameSettings["SceneManager"]["MainScene"] = sceneUUID;
	gameSettings["Camera3D"]["MainCamera"] = cameraGOUUID;
}

void PanelBuild::copyAllFolderMT(const char* path) {
	std::string _path = path;
	App->fs->CreateDirectoryA((buildName + "/" + _path).c_str());
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	App->fs->DiscoverFiles(path, files, dirs);

	// We recursively process another folder in another task
	for (int i = 0; i < dirs.size(); ++i)
		App->threading->ADDTASK(this, PanelBuild::copyAllFolderMT, (_path + "/" + dirs[i]).c_str());

	for (int i = 0; i < files.size(); ++i) {
		App->fs->Copy((_path + files[i]).c_str(), (buildName + "/" + _path + files[i]).c_str());
	}
}
