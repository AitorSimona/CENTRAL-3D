#include "ModuleEditorUI.h"
#include "EngineApplication.h"
#include "Panels.h"
#include "Imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/ImGuizmo/ImGuizmo.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )

ModuleEditorUI::ModuleEditorUI(bool start_enabled) : Module(start_enabled) {
	name = "EditorUI";
}

ModuleEditorUI::~ModuleEditorUI() {
}

bool ModuleEditorUI::Init(BrokenEngine::json file) {

	// --- Create UI Panels ---
	panelSettings = new PanelSettings("Settings");
	EngineApp->gui->AddPanel(panelSettings);

	panelAbout = new PanelAbout("About");
	EngineApp->gui->AddPanel(panelAbout);

	panelConsole = new PanelConsole("Console");
	EngineApp->gui->AddPanel(panelConsole);

	panelInspector = new PanelInspector("Inspector");
	EngineApp->gui->AddPanel(panelInspector);

	panelHierarchy = new PanelHierarchy("Hierarchy");
	EngineApp->gui->AddPanel(panelHierarchy);

	panelScene = new PanelScene("Scene");
	EngineApp->gui->AddPanel(panelScene);

	panelToolbar = new PanelToolbar("Toolbar");
	EngineApp->gui->AddPanel(panelToolbar);

	panelProject = new PanelProject("Project");
	EngineApp->gui->AddPanel(panelProject);

	panelShaderEditor = new PanelShaderEditor("ShaderEditor");
	EngineApp->gui->AddPanel(panelShaderEditor);

	panelResources = new PanelResources("Resources");
	EngineApp->gui->AddPanel(panelResources);

	EngineApp->gui->LoadStatus(file);

	return true;
}

update_status ModuleEditorUI::Update(float dt) {
	//// --- Create Main Menu Bar ---

	if (ImGui::BeginMainMenuBar())
	{
	
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					return UPDATE_STOP;
				}

				if (ImGui::MenuItem("Save Scene"))
				{
					//App->scene_manager->SaveScene();
				}

				if (ImGui::MenuItem("Load Scene"))
				{
					//App->scene_manager->LoadScene();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::BeginMenu("3D Object"))
				{

					if (ImGui::MenuItem("Cube"))
					{
						EngineApp->scene_manager->LoadCube();
					}
					if (ImGui::MenuItem("Sphere"))
					{
						EngineApp->scene_manager->LoadSphere();
					}
					if (ImGui::MenuItem("Empty Game Object"))
					{
						EngineApp->scene_manager->CreateEmptyGameObject();
					}
					if (ImGui::MenuItem("Camera"))
					{
						BrokenEngine::GameObject* cam = EngineApp->scene_manager->CreateEmptyGameObject();
						BrokenEngine::ComponentCamera* camera = (BrokenEngine::ComponentCamera*)cam->AddComponent(BrokenEngine::Component::ComponentType::Camera);
						cam->AddComponent(BrokenEngine::Component::ComponentType::MeshRenderer);
						camera->SetFarPlane(10);
					}

					if (ImGui::MenuItem("Redo Octree"))
					{
						EngineApp->scene_manager->RedoOctree();
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Project"))
				{
					panelProject->OnOff();
				}

				if (ImGui::MenuItem("Settings"))
				{
					panelSettings->OnOff();
				}

				if (ImGui::MenuItem("Inspector"))
				{
					panelInspector->OnOff();
				}

				if (ImGui::MenuItem("Hierarchy"))
				{
					panelHierarchy->OnOff();
				}

				if (ImGui::MenuItem("Scene"))
				{
					panelScene->OnOff();
				}

				if (ImGui::MenuItem("Toolbar"))
				{
					panelToolbar->OnOff();
				}

				if (ImGui::MenuItem("ShaderEditor"))
				{
					panelShaderEditor->OnOff();
				}

				if (ImGui::MenuItem("Resources"))
				{
					panelResources->OnOff();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("ImGui Demo"))
				{
					show_demo_window = !show_demo_window;
				}

				if (ImGui::MenuItem("Documentation"))
				{
					EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/wiki");
				}

				if (ImGui::MenuItem("Download latest"))
				{
					EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/releases");
				}

				if (ImGui::MenuItem("Report a bug"))
				{
					EngineApp->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/issues");
				}

				if (ImGui::MenuItem("About"))
				{
					panelAbout->OnOff();
				}

				ImGui::EndMenu();
			}


			ImGui::EndMainMenuBar();
	}

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	return UPDATE_CONTINUE;
}
