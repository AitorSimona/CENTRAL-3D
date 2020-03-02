#include "PanelBuild.h"
//#include "JSONLoader.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Globals.h"
#include "ModuleThreading.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include <queue>
#include "Component.h"
#include "Imgui/imgui.h"
#include "ResourceScene.h"

PanelBuild::PanelBuild(char* name) : Panel(name){
	buildName = "Broken Engine Game";
	enabled = false;
}

PanelBuild::~PanelBuild() {
}

bool PanelBuild::Draw() {
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		ImGui::Separator();
		ImGui::Text("Build name:");
		static char buildNameBuffer[100];
		strcpy_s(buildNameBuffer, 100, buildName.c_str());
		if (ImGui::InputText("", buildNameBuffer, 100,  ImGuiInputTextFlags_AutoSelectAll))
			buildName = buildNameBuffer;
		ImGui::Separator();
		ImGui::Text("Main scene:");
		ImGui::Text("Currently multiple scenes are not supported, main scene is scene.");
		scenePath = App->scene_manager->currentScene->GetResourceFile();
		//ResourceScene* selected_scene = App->resources->scenes[0];
		//static ImGuiComboFlags flags = 0;
		//if (ImGui::BeginCombo("##Scenes Combo", "Main Scene", flags)) // The second parameter is the label previewed before opening the combo.
		//{
		//	for (int n = 0; n < App->resources->scenes.size(); n++) {
		//		bool is_selected = (selected_scene == App->resources->scenes[n]);
		//		if (ImGui::Selectable(App->resources->scenes[n]->GetName(), is_selected)) {
		//			selected_scene = App->resources->scenes[n];
		//			sceneUID = selected_scene->GetUID();
		//			//MYTODO: scene should have its main camera saved we should set it here once it has it
		//		}
		//		if (is_selected)
		//			ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		//	}
		//	ImGui::EndCombo();
		//}
		ImGui::Separator();
		ImGui::Text("Main camera:");
		if (cameras.size() > 0) {
			static ImGuiComboFlags flags = 0;
			if (ImGui::BeginCombo("##Scenes Combo", selectedCamera->GetName().c_str(), flags)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < cameras.size(); n++) {
					bool is_selected = (selectedCamera == cameras[n]);
					if (ImGui::Selectable(cameras[n]->GetName().c_str(), is_selected)) {
						selectedCamera = cameras[n];
						//cameraName = selectedCamera->GetName();
						//MYTODO: scene should have its main camera saved we should set it here once it has it
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();
			if (ImGui::Button("Build the game!")) {
				makeBuild();
			}
		}
		else
			ImGui::Text("You have no cameras in the scene.");


		ImGui::End();
	}



	return true;
}

inline void PanelBuild::OnOff() {
	if (!enabled) {
		enabled = true;
		//App->scene_manager->SaveScene(App->scene_manager->currentScene);
		findCameras();
		if (cameras.size() > 0)
			selectedCamera = cameras[0];
		else
			selectedCamera = nullptr;
	}
	else
		enabled = false;
}

inline void PanelBuild::SetOnOff(bool set) {
	if (set) {
		//App->scene_manager->SaveScene(App->scene_manager->currentScene);
		findCameras();
		if (cameras.size() > 0)
			selectedCamera = cameras[0];
		else
			selectedCamera = nullptr;
	}
	enabled = set;
}

//MYTODO: this should not be needed, when scene saves its own main camera
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


	// Create build folders
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

		if (extension == "dll" || extension == "ini" || extension == "meta")
			App->fs->Copy(files[i].c_str(), (buildName + "/" + files[i]).c_str());
	}
	static const char* directories[] = { ASSETS_FOLDER, SETTINGS_FOLDER, LIBRARY_FOLDER, TEXTURES_FOLDER, MESHES_FOLDER, SCENES_FOLDER,
		MODELS_FOLDER, SHADERS_FOLDER, SCRIPTS_FOLDER, SHADERS_ASSETS_FOLDER, SOUNDS_FOLDER, ANIMATIONS_FOLDER, BONES_FOLDER};

	for (int i = 0; i < IM_ARRAYSIZE(directories); ++i) {
		App->fs->CreateDirectoryA((buildName + "/" + directories[i]).c_str());
		App->threading->ADDTASK(this, PanelBuild::copyAllFolderMT, directories[i]);
	}

	//We wait for the module threading to finish tasks
	App->threading->FinishProcessing();

	std::string settingspath = buildName + "/Settings/GameConfig.json";
	//We write our settings to gameSettings.
	json gameSettings = App->GetDefaultGameConfig();
	gameSettings["Application"]["Title"] = buildName;
	gameSettings["SceneManager"]["MainScene"] = scenePath;
	gameSettings["Camera3D"]["MainCamera"] = selectedCamera->GetName();

	App->GetJLoader()->Save(settingspath.c_str(), gameSettings);

	SetOnOff(false);
}

void PanelBuild::copyAllFolderMT(const char* path) {
	std::string _path = path;
	//App->fs->CreateDirectoryA((buildName + "/" + _path).c_str());
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	App->fs->DiscoverFiles(path, files, dirs);

	//// We recursively process another folder in another task
	//for (int i = 0; i < dirs.size(); ++i)
	//	App->threading->ADDTASK(this, PanelBuild::copyAllFolderMT, (_path + "/" + dirs[i]).c_str());

	for (int i = 0; i < files.size(); ++i) {
		App->fs->Copy((_path + files[i]).c_str(), (buildName + "/" + _path + "/" + files[i]).c_str());
	}
}
