#include "PanelBuild.h"
#include "EngineApplication.h"

#include <queue>
#include "Imgui/imgui.h"

// -- Modules --
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleFilesystem.h"
#include "ModuleThreading.h"

// -- Resources --
#include "GameObject.h"
#include "ResourceScene.h"


PanelBuild::PanelBuild(char* name) : Panel(name){
	buildName = "Broken Engine Game";
	enabled = false;
}

PanelBuild::~PanelBuild() {
}

bool PanelBuild::Draw() {
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin(name, &enabled, settingsFlags)) {
		#ifdef _DEBUG
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75, 0, 0, 255));
		ImGui::Text("Hey, it looks like you might be doing a build in Debug mode!");
		ImGui::Text("Do make sure you have compiled in release first or BEG.exe will be non-existant!");
		ImGui::PopStyleColor();
		#endif
		ImGui::Separator();
		ImGui::Text("Build name:");
		static char buildNameBuffer[100];
		strcpy_s(buildNameBuffer, 100, buildName.c_str());
		if (ImGui::InputText("", buildNameBuffer, 100,  ImGuiInputTextFlags_AutoSelectAll))
			buildName = buildNameBuffer;
		ImGui::Separator();
		ImGui::Text("Main scene:");
		ImGui::Text("Currently multiple scenes are not supported, main scene is scene.");
		sceneName = EngineApp->scene_manager->currentScene->GetName();
		//ResourceScene* selected_scene = EngineApp->resources->scenes[0];
		//static ImGuiComboFlags flags = 0;
		//if (ImGui::BeginCombo("##Scenes Combo", "Main Scene", flags)) // The second parameter is the label previewed before opening the combo.
		//{
		//	for (int n = 0; n < EngineApp->resources->scenes.size(); n++) {
		//		bool is_selected = (selected_scene == EngineApp->resources->scenes[n]);
		//		if (ImGui::Selectable(EngineApp->resources->scenes[n]->GetName(), is_selected)) {
		//			selected_scene = EngineApp->resources->scenes[n];
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
			if (ImGui::BeginCombo("##Scenes Combo", selectedCamera->GetName(), flags)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < cameras.size(); n++) {
					bool is_selected = (selectedCamera == cameras[n]);
					if (ImGui::Selectable(cameras[n]->GetName(), is_selected)) {
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
			if (ImGui::BeginPopup("Build already exists!")) {
				ImGui::Text("The build \"%s\" already exists!", buildName.c_str());
				ImGui::Text("Should we delete it?");

				if (ImGui::Button("Yes##deleteBuild")) {
					EngineApp->fs->DeleteDirectoryAndContents(buildName.c_str());
					makeBuild();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No##deleteBuild")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (ImGui::Button("Build the game!")) {
				if (EngineApp->fs->Exists((BUILDS_FOLDER + buildName).c_str()))
					ImGui::OpenPopup("Build already exists!");
				else
					makeBuild();
			}
		}
		else
			ImGui::Text("You have no cameras in the scene.");

		ImGui::Checkbox("Make the game build debuggable (on the Lua part)", &Activate_Debug);
	}
	ImGui::End();

	return true;
}

inline void PanelBuild::OnOff() {
	if (!enabled) {
		enabled = true;
		//EngineApp->scene_manager->SaveScene(EngineApp->scene_manager->currentScene);
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
		//EngineApp->scene_manager->SaveScene(EngineApp->scene_manager->currentScene);
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
	std::queue<Broken::GameObject*> GOqueue;
	GOqueue.push(EngineApp->scene_manager->GetRootGO());

	while (!GOqueue.empty()) {
		Broken::GameObject* current = GOqueue.front();
		GOqueue.pop();

		Broken::Component* cameraComponent = current->HasComponent(Broken::Component::ComponentType::Camera);
		if (cameraComponent != nullptr)
			cameras.push_back(current);

		for (int i = 0; i < current->childs.size(); ++i)
			GOqueue.push(current->childs[i]);
	}
	
}

void PanelBuild::makeBuild() {

	// Save the scene
	EngineApp->scene_manager->SaveScene(EngineApp->scene_manager->currentScene);

	std::string buildFolder = BUILDS_FOLDER + buildName + '/';

	// Create build folders
	EngineApp->fs->CreateDirectoryA(buildFolder.c_str());
	EngineApp->fs->CreateDirectoryA((buildFolder + LIBRARY_FOLDER).c_str());
	EngineApp->fs->CreateDirectoryA((buildFolder + ASSETS_FOLDER).c_str());
	EngineApp->fs->CreateDirectoryA((buildFolder + SETTINGS_FOLDER).c_str());
	EngineApp->fs->CreateDirectoryA((buildFolder + LUA_GLOBALS).c_str());
	if(Activate_Debug)
	EngineApp->fs->CreateDirectoryA((buildFolder + LUA_DEBUG).c_str());

	//We copy the executable
	EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, GAME_EXE, buildFolder.c_str());

	const std::vector<std::string>& files = EngineApp->fs->ExDiscoverFiles("");

	//We copy all dlls and .ini
	std::string extension;
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		extension = ((*it).substr((*it).find_last_of(".") + 1));

		// We normalize it
		for (std::string::iterator c = extension.begin(); c != extension.end(); ++c) {
			if (*c == '\\')
				*c = '/';
			else
				*c = std::tolower(*c);
		}

		if (extension == "dll" || extension == "ini" || extension == "meta")
			EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, (*it).c_str(), buildFolder.c_str());
	}

	// We create the folder structure and get all files we need to copy
	std::vector<std::string> filesToCopy;
	createFoldersAndRetrieveFiles(ASSETS_FOLDER, (buildFolder + ASSETS_FOLDER).c_str(), filesToCopy);
	createFoldersAndRetrieveFiles(LIBRARY_FOLDER, (buildFolder + LIBRARY_FOLDER).c_str(), filesToCopy);
	createFoldersAndRetrieveFiles(LUA_GLOBALS, (buildFolder + LUA_GLOBALS).c_str(), filesToCopy);
	if(Activate_Debug)
	createFoldersAndRetrieveFiles(LUA_DEBUG, (buildFolder + LUA_DEBUG).c_str(), filesToCopy);

	for (std::vector<std::string>::const_iterator it = filesToCopy.begin(); it != filesToCopy.end(); ++it)
		EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, (*it).c_str(), buildFolder.c_str());

	//We wait for the module threading to finish tasks
	EngineApp->threading->FinishProcessing();
	EngineApp->fs->DeleteArray(files);
	filesToCopy.clear();


	std::string settingspath = buildFolder + SETTINGS_FOLDER + "GameConfig.json";
	//We write our settings to gameSettings.
	Broken::json gameSettings;
	EngineApp->GetDefaultGameConfig(gameSettings);
	gameSettings["Application"]["Title"] = buildName;
	gameSettings["SceneManager"]["MainScene"] = sceneName;
	gameSettings["Camera3D"]["MainCamera"] = selectedCamera->GetName();
	gameSettings["Window"]["sceneX"] = EngineApp->gui->sceneWidth;
	gameSettings["Window"]["sceneY"] = EngineApp->gui->sceneHeight;
	gameSettings["Scripting"]["LUA_Debug_Game"] = Activate_Debug;

	EngineApp->SaveForBuild(gameSettings, settingspath.c_str());
	SetOnOff(false);
}

void PanelBuild::copyFile(const char* path, const char* buildFolder) {
	std::string new_path = buildFolder;
	new_path += path;
	EngineApp->fs->Copy(path, new_path.c_str());
}

void PanelBuild::createFoldersAndRetrieveFiles(const char* path, const char* newPath, std::vector<std::string>& outFiles) {
	const std::vector<std::string>& folders = EngineApp->fs->ExDiscoverDirectories(path);
	const std::vector<std::string>& files = EngineApp->fs->ExDiscoverFiles(path);


	for (std::vector<std::string>::const_iterator it = folders.begin(); it != folders.end(); ++it) {
		EngineApp->fs->CreateDirectoryA((newPath + *it).c_str());
		createFoldersAndRetrieveFiles((path + *it + "/").c_str(), (newPath + *it + "/").c_str(), outFiles);
	}

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
		outFiles.push_back(path + *it);

	EngineApp->fs->DeleteArray(folders);
	EngineApp->fs->DeleteArray(files);
}