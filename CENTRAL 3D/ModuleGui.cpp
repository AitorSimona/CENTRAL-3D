#include "Application.h"
#include "Globals.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "Source/Imgui/imgui.h"
#include "Source/imgui/imgui_impl_sdl.h"
#include "Source/imgui/imgui_impl_opengl3.h"

static bool show_demo_window = true;
bool show_another_window = true;

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

	if (show_another_window)
	{
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
			ImGui::EndMainMenuBar();
		}

		//ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//ImGui::Text("Hello from another window!");
		//if (ImGui::Button("Close Me"))
		//	show_another_window = false;
		//ImGui::End();
	}

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
}