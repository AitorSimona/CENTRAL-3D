#include "PanelBuild.h"
#include "EngineApplication.h"

#include <cctype>
#include <queue>
#include "Imgui/imgui.h"

// -- Modules --
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleFilesystem.h"
#include "ModuleThreading.h"
#include "ModuleResourceManager.h"

// -- Resources --
#include "GameObject.h"
#include "ResourceScene.h"
#include "ResourceNavMesh.h"
#include "ResourceScript.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"


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
			if (ImGui::BeginPopup("Build folder already exists!")) {
				ImGui::Text("The folder \"%s\" already exists!", buildName.c_str());
				ImGui::Text("Should we delete it?");

				if (ImGui::Button("Yes##deleteBuild")) {
					EngineApp->fs->DeleteDirectoryAndContents(buildName.c_str());
					makeBuild();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("No##deleteBuild")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (ImGui::Button("Build the game!")) {
				if (EngineApp->fs->Exists(buildName.c_str()))
					ImGui::OpenPopup("Build folder already exists!");
				else
					makeBuild();
			}
		}
		else
			ImGui::Text("You have no cameras in the scene.");
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
	
	std::vector<std::string> paths;

	//Get assets path
	for (std::map<uint, Broken::ResourceScene*>::const_iterator it = EngineApp->resources->scenes.begin(); it != EngineApp->resources->scenes.end(); ++it)
		if (EngineApp->fs->Exists((*it).second->GetOriginalFile()))
			paths.push_back((*it).second->GetOriginalFile());

	for (std::map<uint, Broken::ResourceNavMesh*>::const_iterator it = EngineApp->resources->navmeshes.begin(); it != EngineApp->resources->navmeshes.end(); ++it)
		if (EngineApp->fs->Exists((*it).second->GetOriginalFile()))
			paths.push_back((*it).second->GetOriginalFile());


	for (std::map<uint, Broken::ResourceScript*>::const_iterator it = EngineApp->resources->scripts.begin(); it != EngineApp->resources->scripts.end(); ++it)
		if (EngineApp->fs->Exists((*it).second->GetOriginalFile()))
			paths.push_back((*it).second->GetOriginalFile());


	for (std::map<uint, Broken::ResourceMaterial*>::const_iterator it = EngineApp->resources->materials.begin(); it != EngineApp->resources->materials.end(); ++it)
		if (EngineApp->fs->Exists((*it).second->GetOriginalFile()))
			paths.push_back((*it).second->GetOriginalFile());

	for (std::map<uint, Broken::ResourceShader*>::const_iterator it = EngineApp->resources->shaders.begin(); it != EngineApp->resources->shaders.end(); ++it)
		if (EngineApp->fs->Exists((*it).second->GetOriginalFile()))
			paths.push_back((*it).second->GetOriginalFile());


	// Create build folder
	EngineApp->fs->CreateDirectoryA(buildName.c_str());

	//We copy the executable
	EngineApp->fs->Copy(GAME_EXE, (buildName + "/" + buildName + ".exe").c_str());

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
			EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, (*it).c_str(), buildName.c_str());
	}

	//@Improvement Sergi: Might want to check the scenes for files used in the library
	//We copy all of the library
	EngineApp->fs->CreateDirectoryA((buildName + "/Library").c_str());
	const std::vector<std::string>& libfolderlist = EngineApp->fs->ExDiscoverDirectories(LIBRARY_FOLDER);

	std::vector<std::string> oldFolders;
	std::vector<std::string> newFolders;
	oldFolders.reserve(libfolderlist.size());
	newFolders.reserve(libfolderlist.size());
	for (std::vector<std::string>::const_iterator it = libfolderlist.begin(); it != libfolderlist.end(); ++it) {
		EngineApp->fs->CreateDirectoryA((buildName + "/Library/" + *it).c_str());
		oldFolders.push_back("Library/" + *it);
		newFolders.push_back(buildName + "/Library/" + *it);
		EngineApp->threading->ADDTASK(EngineApp->fs, Broken::ModuleFileSystem::CopyDirectoryandContents, oldFolders.back().c_str(), newFolders.back().c_str(), false);
	}

	//Create directories for files
	createFileFolders(paths);
	std::vector<std::string> metas;

	// Copy all files to the build 
	metas.reserve(paths.size());
	for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it) {
		EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, (*it).c_str(), buildName.c_str());
		metas.push_back(*it + ".meta");
		EngineApp->threading->ADDTASK(this, PanelBuild::copyFile, metas.back().c_str(), buildName.c_str());
	}

	// Create settings folder
	EngineApp->fs->CreateDirectoryA((buildName + "/Settings").c_str());

	std::string settingspath = buildName + "/Settings/GameConfig.json";
	//We write our settings to gameSettings.
	Broken::json gameSettings;
	EngineApp->GetDefaultGameConfig(gameSettings);
	gameSettings["Application"]["Title"] = buildName;
	gameSettings["SceneManager"]["MainScene"] = sceneName;
	gameSettings["Camera3D"]["MainCamera"] = selectedCamera->GetName();
	gameSettings["Window"]["sceneX"] = EngineApp->gui->sceneWidth;
	gameSettings["Window"]["sceneY"] = EngineApp->gui->sceneHeight;

	EngineApp->SaveForBuild(gameSettings, settingspath.c_str());

	//We wait for the module threading to finish tasks
	EngineApp->threading->FinishProcessing();
	paths.clear();
	metas.clear();
	oldFolders.clear();
	newFolders.clear();

	// We delete the arrays we created to store the files
	EngineApp->fs->DeleteArray(files);
	EngineApp->fs->DeleteArray(libfolderlist);
	SetOnOff(false);
}

//@Improvement Sergi: Maybe optimize this
void PanelBuild::createFileFolders(const std::vector<std::string>& files) {
	std::vector<std::string> c_directories;
	std::vector<std::string> createdDirs;
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		getAllFolders(*it, c_directories);
		for (std::vector<std::string>::reverse_iterator dir = c_directories.rbegin(); dir != c_directories.rend(); ++dir) {
			bool included = false;
			for (int i = 0; i < createdDirs.size() && !included; ++i)
				included = createdDirs[i] == *dir;
			if (!included) {
				EngineApp->fs->CreateDirectoryA((buildName + '/' + *dir).c_str());
				createdDirs.push_back(*dir);
			}
		}
	}
}

inline void PanelBuild::getAllFolders(const std::string& file, std::vector<std::string>& outDirs) {
	outDirs.clear();
	// --- Count until the last / ---
	uint count = file.find_last_of("/");
	std::string directory = file;

	while (count != std::string::npos) {
		directory = directory.substr(0, count);
		outDirs.push_back(directory);
		count = directory.find_last_of("/");
	}
}

void PanelBuild::copyAllFolder(const char* path, const char* build) {
	const std::vector<std::string>& files = EngineApp->fs->ExDiscoverFiles(path);

	std::string curr_path;
	std::string new_path;

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		curr_path = path + *it;
		new_path = std::string(build) + "/" + path + *it;
		EngineApp->fs->Copy(curr_path.c_str(), new_path.c_str());
	}

	EngineApp->fs->DeleteArray(files);
}

void PanelBuild::copyFile(const char* path, const char* build) {
	std::string new_path = build;
	new_path += "/";
	new_path += path;
	EngineApp->fs->Copy(path, new_path.c_str());
}