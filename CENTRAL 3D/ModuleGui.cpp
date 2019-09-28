#include "Application.h"
#include "Globals.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "Source/Imgui/imgui.h"
#include "Source/imgui/imgui_impl_sdl.h"
#include "Source/imgui/imgui_impl_opengl3.h"
#include "Source/Imgui/imgui_internal.h"
#include "Source/Imgui/ImGuizmo/ImGuizmo.h"

#include "Source/glew/include/GL/glew.h"



ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Init()
{


	return true;
}

bool ModuleGui::Start()
{
	bool ret = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Window Docking (Under Active Development)
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Deactivated because of lib crash when resizing window out of Main window bounds

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	return ret;
}

update_status ModuleGui::PreUpdate(float dt)
{  
	// Start the frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();
	const char* testt = "Hello World";
	teest.Load(testt);

	// Begin dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	DockSpace();

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt)
{

	// Menu Bar
	/*if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Examples"))
		{
			ImGui::MenuItem("Quit", NULL);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Quit", "Alt+F4"))
		{

		}

		ImGui::EndMenuBar();

	}*/



	if (ImGui::BeginMainMenuBar())
	{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::Button("Quit"))
				{
					return UPDATE_STOP;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::Button("Console"))
				{
					
				}

				if (ImGui::Button("Configuration"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::Button("ImGui Demo"))
				{
					show_demo_window = !show_demo_window;
				}

				if (ImGui::Button("Documentation"))
				{
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/wiki");
				}

				if (ImGui::Button("Download latest"))
				{
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/releases");
				}

				if (ImGui::Button("Report a bug"))
				{
					RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D/issues");
				}

				if (ImGui::Button("About"))
				{

				}

				ImGui::EndMenu();
			}


			ImGui::EndMainMenuBar();
	}

		//ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//ImGui::Text("Hello from another window!");
		//if (ImGui::Button("Close Me"))
		//	show_another_window = false;
		//ImGui::End();
	

	//if (ImGui::BeginMainMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		ShowExampleMenuFile();
	//		ImGui::EndMenu();
	//	}
	//	if (ImGui::BeginMenu("Edit"))
	//	{
	//		if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
	//		if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
	//		ImGui::Separator();
	//		if (ImGui::MenuItem("Cut", "CTRL+X")) {}
	//		if (ImGui::MenuItem("Copy", "CTRL+C")) {}
	//		if (ImGui::MenuItem("Paste", "CTRL+V")) {}
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMainMenuBar();
	//}


	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	return UPDATE_CONTINUE;
}

update_status ModuleGui::PostUpdate(float dt)
{
	// End dock space

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	ImGui::End();

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	bool ret = true;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

void ModuleGui::Draw() const
{
	// Render
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
	/*ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);*/

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	static bool p_open = true;
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

void ModuleGui::RequestBrowser(const char * url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
