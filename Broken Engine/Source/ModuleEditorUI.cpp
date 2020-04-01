#include "ModuleEditorUI.h"
#include "EngineApplication.h"
#include "Panels.h"
#include "Imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/ImGuizmo/ImGuizmo.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
//#include "mmgr/mmgr.h"

ModuleEditorUI::ModuleEditorUI(bool start_enabled) : Module(start_enabled) {
	name = "EditorUI";
}

ModuleEditorUI::~ModuleEditorUI() {
}

bool ModuleEditorUI::Init(Broken::json& file) {

	ImGui::SetAllocatorFunctions(EngineApp->gui->GetImGuiAlloc(), EngineApp->gui->GetImGuiFree());

	// --- Create UI Panels ---
	panelSettings = new PanelSettings("Settings");
	panels.push_back(panelSettings);

	panelAbout = new PanelAbout("About");
	panels.push_back(panelAbout);

	panelConsole = new PanelConsole("Console");
	panels.push_back(panelConsole);

	panelInspector = new PanelInspector("Inspector");
	panels.push_back(panelInspector);

	panelRendering = new PanelRendering("Rendering");
	panels.push_back(panelRendering);

	panelHierarchy = new PanelHierarchy("Hierarchy");
	panels.push_back(panelHierarchy);

	panelScene = new PanelScene("Scene");
	panels.push_back(panelScene);

	panelToolbar = new PanelToolbar("Toolbar");
	panels.push_back(panelToolbar);

	panelProject = new PanelProject("Project");
	panels.push_back(panelProject);

	panelShaderEditor = new PanelShaderEditor("ShaderEditor");
	panels.push_back(panelShaderEditor);

	panelResources = new PanelResources("Resources");
	panels.push_back(panelResources);

	panelBuild = new PanelBuild("Build");
	panels.push_back(panelBuild);

	panelPhysics = new PanelPhysics("Physics");
	panels.push_back(panelPhysics);

	LoadStatus(file);

	return true;
}

bool ModuleEditorUI::Start() {
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());
	return true;
}

update_status ModuleEditorUI::Update(float dt) {
	//// --- Create Main Menu Bar ---
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit")) {
				return UPDATE_STOP;
			}

			if (ImGui::MenuItem("Save Scene")) {
				EngineApp->scene_manager->SaveScene(EngineApp->scene_manager->currentScene);
			}

			if (ImGui::MenuItem("Load Scene")) {
				//App->scene_manager->SetActiveScene();
			}

			if (ImGui::MenuItem("Build Game")) {
				panelBuild->SetOnOff(true);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject")) {
			if (ImGui::BeginMenu("3D Object")) {
				if (ImGui::MenuItem("Empty Game Object")) {
					Broken::GameObject* go = EngineApp->scene_manager->CreateEmptyGameObject();
					//App->scene_manager->currentScene->NoStaticGameObjects[go->GetUID()] = go;
				}


				if (ImGui::MenuItem("Plane")) {
					Broken::GameObject* obj = EngineApp->scene_manager->LoadPlane();
					obj->GetComponent<Broken::ComponentTransform>()->SetRotation({ -90, 0, 0 });
					obj->GetComponent < Broken::ComponentTransform > ()->Scale(10, 10, 10);
				}

				if (ImGui::MenuItem("Cube"))
					EngineApp->scene_manager->LoadCube();

				if (ImGui::MenuItem("Disk"))
					EngineApp->scene_manager->LoadDisk();

				if (ImGui::MenuItem("Cylinder"))
					EngineApp->scene_manager->LoadCylinder()->GetComponent<Broken::ComponentTransform>()->SetRotation({ -90, 0, 0 });

				if (ImGui::MenuItem("Capsule"))
					EngineApp->scene_manager->LoadCapsule();

				if (ImGui::MenuItem("Sphere"))
					EngineApp->scene_manager->LoadSphere();

				if (ImGui::MenuItem("Camera")) {
					Broken::GameObject* cam = EngineApp->scene_manager->CreateEmptyGameObject();
					//App->scene_manager->currentScene->NoStaticGameObjects[cam->GetUID()] = cam;

					Broken::ComponentCamera* camera = (Broken::ComponentCamera*)cam->AddComponent(Broken::Component::ComponentType::Camera);
					cam->AddComponent(Broken::Component::ComponentType::MeshRenderer);
					camera->SetFarPlane(10);
				}

				if (ImGui::MenuItem("Redo Octree"))
					EngineApp->scene_manager->RedoOctree();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("UI Elements")) 
			{
				if (ImGui::MenuItem("Canvas")) 
				{
					Broken::GameObject* canvas_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentCanvas* camera = (Broken::ComponentCanvas*)canvas_go->AddComponent(Broken::Component::ComponentType::Canvas);
				}
				if (ImGui::MenuItem("Image")) 
				{
					Broken::GameObject* image_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentImage* image = (Broken::ComponentImage*)image_go->AddComponent(Broken::Component::ComponentType::Image);
				}
				if (ImGui::MenuItem("Text")) 
				{
					Broken::GameObject* text_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentText* text = (Broken::ComponentText*)text_go->AddComponent(Broken::Component::ComponentType::Text);
				}
				if (ImGui::MenuItem("Button")) 
				{
					Broken::GameObject* button_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentText* button = (Broken::ComponentText*)button_go->AddComponent(Broken::Component::ComponentType::Button);
				}
				if (ImGui::MenuItem("Checkbox")) 
				{
				}
				if (ImGui::MenuItem("Input Text")) 
				{
				}
				if (ImGui::MenuItem("Progress Bar"))
				{
					Broken::GameObject* bar_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentProgressBar* bar = (Broken::ComponentProgressBar*)bar_go->AddComponent(Broken::Component::ComponentType::ProgressBar);
				}
				if (ImGui::MenuItem("Circular Bar"))
				{
					Broken::GameObject* cbar_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentProgressBar* cbar = (Broken::ComponentProgressBar*)cbar_go->AddComponent(Broken::Component::ComponentType::CircularBar);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Lights"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::DIRECTIONAL);
				}

				if (ImGui::MenuItem("Pointlight"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::POINTLIGHT);
				}

				if (ImGui::MenuItem("Spotlight"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::SPOTLIGHT);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("Project")) {
				panelProject->OnOff();
			}

			if (ImGui::MenuItem("Settings")) {
				panelSettings->OnOff();
			}

			if (ImGui::MenuItem("Inspector")) {
				panelInspector->OnOff();
			}

			if (ImGui::MenuItem("Rendering")) {
				panelRendering->OnOff();
			}

			if (ImGui::MenuItem("Hierarchy")) {
				panelHierarchy->OnOff();
			}

			if (ImGui::MenuItem("Scene")) {
				panelScene->OnOff();
			}

			if (ImGui::MenuItem("Toolbar")) {
				panelToolbar->OnOff();
			}

			if (ImGui::MenuItem("ShaderEditor")) {
				panelShaderEditor->OnOff();
			}

			if (ImGui::MenuItem("Resources")) {
				panelResources->OnOff();
			}

			if (ImGui::MenuItem("Physics")) {
				panelPhysics->OnOff();
			}

			if (ImGui::MenuItem("Console")) {
				panelConsole->OnOff();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("ImGui Demo")) {
				show_demo_window = !show_demo_window;
			}

			if (ImGui::MenuItem("Documentation")) {
				EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/wiki");
			}

			if (ImGui::MenuItem("Download latest")) {
				EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/releases");
			}

			if (ImGui::MenuItem("Report a bug")) {
				EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/issues");
			}

			if (ImGui::MenuItem("About")) {
				panelAbout->OnOff();
			}

			if(ImGui::MenuItem("DoGame"))
				EngineApp->gui->RequestBrowser("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
			if (ImGui::MenuItem("DoFrozenGame"))
				EngineApp->gui->RequestBrowser("https://www.youtube.com/watch?v=L0MK7qz13bU&feature=youtu.be&t=126");
			if (ImGui::MenuItem("I Want to Commit Suicide"))
				EngineApp->gui->RequestBrowser("https://drive.google.com/file/d/1alYVc1pEzjF6crE49_FW5FYaiuwmZ2yN/view?usp=sharing");

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);


	EngineApp->gui->isUsingGuizmo = ImGuizmo::IsUsing();
	return UPDATE_CONTINUE;
}

update_status ModuleEditorUI::PostUpdate(float dt) {
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());
	for (uint i = 0; i < panels.size(); ++i) {
	if (panels[i]->IsEnabled())
		panels[i]->Draw();
	}
	return UPDATE_CONTINUE;
}

bool ModuleEditorUI::CleanUp() {
	for (int i = 0; i < panels.size(); i++)
		delete panels[i];

	panelSettings = nullptr;
	panelAbout = nullptr;
	panelConsole = nullptr;
	panelInspector = nullptr;
	panelRendering = nullptr;
	panelHierarchy = nullptr;
	panelScene = nullptr;
	panelToolbar = nullptr;
	panelProject = nullptr;
	panelShaderEditor = nullptr;
	panelResources = nullptr;
	panelBuild = nullptr;
	panelPhysics = nullptr;

	return true;
}

const Broken::json& ModuleEditorUI::SaveStatus() const {
	//MYTODO: Added exception for Build because Build should never be enabled at start
	//maybe we should call SaveStatus on every panel
	static Broken::json config;
	for (uint i = 0; i < panels.size(); ++i) {
		if (panels[i]->GetName() == "Build")
			config[panels[i]->GetName()] = false;
		else
			config[panels[i]->GetName()] = panels[i]->IsEnabled();
	} 
	return config;
};

void ModuleEditorUI::LoadStatus(const Broken::json& file) {

	for (uint i = 0; i < panels.size(); ++i) {

		if (file[name].find(panels[i]->GetName()) != file[name].end()) {
			panels[i]->SetOnOff(file[name][panels[i]->GetName()]);
		}
		else
			EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Could not find sub-node %s in GUI JSON Node, please check JSON EditorConfig", panels[i]->GetName());
	}
}
