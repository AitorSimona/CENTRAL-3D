#include "PanelBuild.h"
//#include "JSONLoader.h"
#include "EngineApplication.h"
#include <queue>
#include "Imgui/imgui.h"

PanelBuild::PanelBuild(char* name) : Broken::Panel(name){
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
			if (ImGui::Button("Build the game!")) {
				EngineApp->scene_manager->SaveScene(EngineApp->scene_manager->currentScene);
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


	// Create build folders
	EngineApp->fs->CreateDirectoryA(buildName.c_str());

	//We copy the executable
	EngineApp->fs->Copy(GAME_EXE, (buildName + "/" + buildName + ".exe").c_str());

	const std::vector<std::string>* files = EngineApp->fs->ExDiscoverFiles("");

	//We copy all dlls and .ini
	for (std::vector<std::string>::const_iterator it = (*files).begin(); it != (*files).end(); ++it) {
		std::string extension = ((*it).substr((*it).find_last_of(".") + 1));
		EngineApp->fs->NormalizePath(extension);

		if (extension == "dll" || extension == "ini" || extension == "meta")
			EngineApp->fs->Copy((*it).c_str(), (buildName + "/" + *it).c_str());
	}

	EngineApp->fs->DeleteArray(files);

	static const char* directories[] = { ASSETS_FOLDER, SETTINGS_FOLDER, LIBRARY_FOLDER, TEXTURES_FOLDER, MESHES_FOLDER, SCENES_FOLDER,
		MODELS_FOLDER, SHADERS_FOLDER, SCRIPTS_FOLDER, SHADERS_ASSETS_FOLDER, SOUNDS_FOLDER, ANIMATIONS_FOLDER, BONES_FOLDER, FONTS_FOLDER, NAVMESH_FOLDER};

	std::shared_ptr<std::string> build = std::make_shared<std::string>(buildName);
	for (int i = 0; i < IM_ARRAYSIZE(directories); ++i) {
		std::shared_ptr<std::string> dir = std::make_shared<std::string>(directories[i]);
		EngineApp->fs->CreateDirectoryA((buildName + "/" + directories[i]).c_str());
		EngineApp->threading->ADDTASK(this, PanelBuild::copyAllFolderMT, dir, build);
	}

	//We wait for the module threading to finish tasks
	EngineApp->threading->FinishProcessing();

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

	SetOnOff(false);
}

void PanelBuild::copyAllFolderMT(std::shared_ptr<std::string> path, std::shared_ptr<std::string> build) {
	const std::vector<std::string>* files = EngineApp->fs->ExDiscoverFiles((*path).c_str());

	std::string curr_path;
	std::string new_path;

	for (std::vector<std::string>::const_iterator it = (*files).begin(); it != (*files).end(); ++it) {
		curr_path = *path + *it;
		new_path = *build + "/" + *path + *it;
		EngineApp->fs->Copy(curr_path.c_str(), new_path.c_str());
	}

	EngineApp->fs->DeleteArray(files);
}
