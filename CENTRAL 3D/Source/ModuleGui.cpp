#include "Application.h"
#include "Globals.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "ModuleTextures.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "Panels.h"

#include "Imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/ImGuizmo/ImGuizmo.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"



ModuleGui::ModuleGui(bool start_enabled) : Module(start_enabled)
{
	name = "GUI";
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Init(json file)
{
	// --- Create UI Panels ---

	panelSettings = new PanelSettings("Settings");
	panels.push_back(panelSettings);

	panelAbout = new PanelAbout("About");
	panels.push_back(panelAbout);

	panelConsole = new PanelConsole("Console");
	panels.push_back(panelConsole);

	panelInspector = new PanelInspector("Inspector");
	panels.push_back(panelInspector);

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

	LoadStatus(file);

	return true;
}

bool ModuleGui::Start()
{
	bool ret = true;

	// --- Initialize ImGui ---

	IMGUI_CHECKVERSION();
	ImGuiContext * context = ImGui::CreateContext();

	if (context)
	{
		CONSOLE_LOG("Successfully created ImGui context");

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Window Docking (Under Active Development)
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Deactivated because of lib crash when resizing window out of Main window bounds

		ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
		ImGui_ImplOpenGL3_Init();

		// Setup style
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::StyleColorsDark();
		style.FrameBorderSize = 1.0f;

		// --- Set style colors ---
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.17f, 0.17f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.33f, 0.33f, 0.33f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.48f, 0.48f, 0.48f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.71f, 0.71f, 0.71f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.33f, 0.33f, 0.33f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.66f, 0.66f, 0.66f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.69f, 0.69f, 0.69f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.74f, 0.74f, 0.74f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.31f, 0.31f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.51f, 0.51f, 0.52f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.61f, 0.61f, 0.61f, 0.70f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.61f, 0.61f, 0.61f, 0.70f);
	}
	else
	{
		ret = false;
		CONSOLE_LOG("|[error]: Could not create ImGui context");
	}

	return ret;
}

update_status ModuleGui::PreUpdate(float dt)
{  
	// --- Start the frame ---

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();


	ImGuiIO& io = ImGui::GetIO();
	capture_keyboard = io.WantCaptureKeyboard;
	capture_mouse = io.WantCaptureMouse;

	// Begin dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		DockSpace();

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt)
{
	// --- Create Main Menu Bar ---

	update_status status = UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Image((ImTextureID)sceneTexID, ImVec2(18, 18), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					return UPDATE_STOP;
				}

				if (ImGui::MenuItem("Save Scene"))
				{
					App->scene_manager->SaveScene(App->scene_manager->currentScene);
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
					if (ImGui::MenuItem("Empty Game Object"))
						App->scene_manager->CreateEmptyGameObject();

					if (ImGui::MenuItem("Plane"))
					{
						GameObject* go = App->scene_manager->LoadPlane();

						// MYTODO: Move this to shape creation
						go->GetComponent<ComponentTransform>()->SetRotation({ -90, 0, 0 });
						go->GetComponent<ComponentTransform>()->Scale(10, 10, 10);
					}
					if (ImGui::MenuItem("Cube"))
						App->scene_manager->LoadCube();

					if (ImGui::MenuItem("Capsule"))
						App->scene_manager->LoadCapsule();

					if (ImGui::MenuItem("Sphere"))
						App->scene_manager->LoadSphere();

					if (ImGui::MenuItem("Camera"))
					{
						GameObject* cam = App->scene_manager->CreateEmptyGameObject();
						ComponentCamera* camera = (ComponentCamera*)cam->AddComponent(Component::ComponentType::Camera);
						cam->AddComponent(Component::ComponentType::MeshRenderer);
						camera->SetFarPlane(10);
					}

					if (ImGui::MenuItem("Redo Octree"))
						App->scene_manager->RedoOctree();

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

				if (ImGui::MenuItem("Settings"))
				{
					panelSettings->OnOff();
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
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/wiki");
				}

				if (ImGui::MenuItem("Download latest"))
				{
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/releases");
				}

				if (ImGui::MenuItem("Report a bug"))
				{
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/issues");
				}

				if (ImGui::MenuItem("About"))
				{
					panelAbout->OnOff();
				}

				ImGui::EndMenu();
			}

			// --- Manage Window through main menu bar Minimize/Maximize-Minimize Size/Close---

			if (App->window->IsBorderless())
			{
				ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 70);

				if (ImGui::ImageButton((ImTextureID)minimizebuttonTexID, ImVec2(14, 14), ImVec2(0, 1), ImVec2(1, 0)))
					App->window->MinimizeWindow();

				if (App->window->IsWindowMaximized())
				{
					if (ImGui::ImageButton((ImTextureID)minimizesizebuttonTexID, ImVec2(14, 14), ImVec2(0, 1), ImVec2(1, 0)))
						App->window->RestoreWindow();
				}
				else
				{
					if (ImGui::ImageButton((ImTextureID)maximizesizebuttonTexID, ImVec2(14, 14), ImVec2(0, 1), ImVec2(1, 0)))
						App->window->MaximizeWindow();
				}

				if (ImGui::ImageButton((ImTextureID)closebuttonTexID, ImVec2(14, 14), ImVec2(0, 1), ImVec2(1, 0)))
					status = UPDATE_STOP;

			}



			ImGui::EndMainMenuBar();
	}


	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	return status;
}

update_status ModuleGui::PostUpdate(float dt)
{
	// --- Iterate panels and draw ---
	for (uint i = 0; i < panels.size(); ++i)
	{
		if (panels[i]->IsEnabled())
			panels[i]->Draw();
	}

	// End dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		ImGui::End();

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	bool ret = true;

	// --- Iterate panels and delete ---
	for (uint i = 0; i < panels.size(); ++i)
	{
		delete panels[i];
		panels[i] = nullptr;
	}

	panelSettings = nullptr;
	panelAbout = nullptr;
	panelConsole = nullptr;
	panelHierarchy = nullptr;
	panelInspector = nullptr;
	panelScene = nullptr;
	panelToolbar = nullptr;
	panelProject = nullptr;
	panelShaderEditor = nullptr;

	// --- Delete editor textures ---
	glDeleteTextures(1, &materialTexID);
	glDeleteTextures(1, &folderTexID);
	glDeleteTextures(1, &defaultfileTexID);
	glDeleteTextures(1, &prefabTexID);
	glDeleteTextures(1, &playbuttonTexID);
	glDeleteTextures(1, &sceneTexID);
	glDeleteTextures(1, &shaderTexID);
	glDeleteTextures(1, &closebuttonTexID);
	glDeleteTextures(1, &minimizebuttonTexID);
	glDeleteTextures(1, &minimizesizebuttonTexID);
	glDeleteTextures(1, &maximizesizebuttonTexID);

	// --- ShutDown ImGui ---
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

void ModuleGui::Draw() const
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}

void ModuleGui::DockSpace() const
{
	// --- Adapt to Window changes like resizing ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));

	static bool p_open = true;
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

void ModuleGui::RequestBrowser(const char * url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}


void ModuleGui::LogFPS(float fps, float ms)
{
	if (panelSettings != nullptr)
		panelSettings->AddFPS(fps, ms);
}

void ModuleGui::SaveStatus(json &file) const  
{
	for (uint i = 0; i < panels.size(); ++i)
		file["GUI"][panels[i]->GetName()] = panels[i]->IsEnabled();
};

void ModuleGui::LoadStatus(const json & file) 
{
	for (uint i = 0; i < panels.size(); ++i)
	{
		if (file["GUI"].find(panels[i]->GetName()) != file["GUI"].end())
			panels[i]->SetOnOff(file["GUI"][panels[i]->GetName()]);
		else
			CONSOLE_LOG("|[error]: Could not find sub-node %s in GUI JSON Node, please check JSON EditorConfig", panels[i]->GetName());
	}
}
void ModuleGui::HandleInput(SDL_Event * event) const
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

bool ModuleGui::IsKeyboardCaptured() const
{
	return capture_keyboard;
}

bool ModuleGui::IsMouseCaptured() const
{
	return capture_mouse;
}

void ModuleGui::CreateIcons()
{
	uint width, height;
	// --- Create Icon textures ---
	folderTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Folder Icon.png", width, height, -1);
	defaultfileTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/DefaultAsset Icon.png", width, height, -1);
	materialTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Material Icon.png", width, height, -1);
	prefabTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Prefab.png", width, height, -1);
	playbuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/PlayButton.png", width, height, -1);
	sceneTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Scene.png", width, height, -1);
	shaderTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ShaderIcon.png", width, height, -1);
	closebuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/CloseIcon.png", width, height, -1);
	minimizebuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/MinimizeIcon.png", width, height, -1);
	minimizesizebuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/MinimizeSizeIcon.png", width, height, -1);
	maximizesizebuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/MaximizeSizeIcon.png", width, height, -1);

	// REMEMBER to gldeletetex them at cleanup!
}

