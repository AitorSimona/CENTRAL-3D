#include "Application.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "GameObject.h"

#include "ResourceScene.h"
#include "Panel.h"
//#include "Button.h"
//#include "CheckBox.h"
//#include "InputText.h"
//#include "ProgressBar.h"
#define NOMINMAX
#include <Windows.h>
#include <functional>

#include "Imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/ImGuizmo/ImGuizmo.h"
#include "ModuleFileSystem.h"

#pragma comment( lib, "SDL/libx86/SDL2.lib" )

#include "OpenGL.h"

#include "mmgr/mmgr.h"


void* ImGuiCustomAllocator(size_t sz, void* user_data) {
	return malloc(sz);
}

void ImGuiCustomDeallocator(void* ptr, void* user_data) {
	free(ptr);
}

using namespace Broken;

ModuleGui::ModuleGui(bool start_enabled) : Module(start_enabled)
{
	name = "GUI";
}

ModuleGui::~ModuleGui()
{}

bool ModuleGui::Init(json& file)
{
	if (!App->fs->Exists("imgui.ini"))
		App->fs->Copy("imgui.ini.bak", "imgui.ini");

	return true;
}

bool ModuleGui::Start()
{
	bool ret = true;

	// --- Set scene width/height to default
	sceneWidth = App->window->GetWindowWidth();
	sceneHeight = App->window->GetWindowHeight();

	// --- Initialize ImGui ---
	IMGUI_CHECKVERSION();
	ImGui::SetAllocatorFunctions(&ImGuiCustomAllocator, &ImGuiCustomDeallocator);
	ctx = ImGui::CreateContext();

	if (ctx)
	{
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Successfully created ImGui context");

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
	}
	else
	{
		ret = false;
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Could not create ImGui context");
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

	// --- Ask ImGui if input should be controlled (not using it though) ---
	ImGuiIO& io = ImGui::GetIO();
	capture_keyboard = io.WantCaptureKeyboard;
	capture_mouse = io.WantCaptureMouse;

	// Begin dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		DockSpace();

	return UPDATE_CONTINUE;
}


update_status ModuleGui::PostUpdate(float dt)
{
	// --- Iterate panels and draw ---
	//for (uint i = 0; i < panels.size(); ++i) {
	//	if (panels[i]->IsEnabled())
	//		panels[i]->Draw();
	//}

	// End dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		ImGui::End();

	return UPDATE_CONTINUE;
}

bool ModuleGui::Stop() {

	// --- ShutDown ImGui ---

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext(ctx);

	return true;
}

bool ModuleGui::CleanUp()
{
	bool ret = true;

	// --- Delete editor textures ---
	glDeleteTextures(1, &folderTexID);
	glDeleteTextures(1, &defaultfileTexID);
	glDeleteTextures(1, &materialTexID);
	glDeleteTextures(1, &prefabTexID);
	glDeleteTextures(1, &playbuttonTexID);
	glDeleteTextures(1, &sceneTexID);
	glDeleteTextures(1, &animatorTexID);
	glDeleteTextures(1, &shaderTexID);
	glDeleteTextures(1, &scriptTexID);
	glDeleteTextures(1, &meshTexID);
	glDeleteTextures(1, &boneTexID);
	glDeleteTextures(1, &animationTexID);
	glDeleteTextures(1, &navmeshTexID);
	// -- Toolbar
	glDeleteTextures(1, &translateTexID);
	glDeleteTextures(1, &rotateTexID);
	glDeleteTextures(1, &scaleTexID);
	glDeleteTextures(1, &toolbarPlayTexID);
	glDeleteTextures(1, &toolbarPauseTexID);
	glDeleteTextures(1, &toolbarPauseTexID);
	//glDeleteTextures(1, &toolbarStopTexID);


	return ret;
}

void ModuleGui::Draw() const
{
	if (this->isEnabled()) {
		if (!App->isGame) {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
		else
			ImGui::EndFrame();
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

ImGuiContext* ModuleGui::getImgUICtx() const {
	return ctx;
}

void ModuleGui::HandleInput(SDL_Event * event) const
{
	if(!App->isGame)
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
	materialTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/MaterialIcon.png", width, height, -1);
	prefabTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Prefab.png", width, height, -1);
	playbuttonTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/PlayButton.png", width, height, -1);
	sceneTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/Scene.png", width, height, -1);
	animatorTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/AnimatorIcon.png", width, height, -1);
	navmeshTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/NavMesh.png", width, height, -1);
	shaderTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ShaderIcon.png", width, height, -1);
	scriptTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ScriptIcon.png", width, height, -1);
	meshTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/MeshIcon.png", width, height, -1);
	boneTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/BoneIcon.png", width, height, -1);
	animationTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/AnimationIcon.png", width, height, -1);

	// -- Toolbar icons
	translateTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/TranslateIcon.png", width, height, -1);
	rotateTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/RotateIcon.png", width, height, -1);
	scaleTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ScaleIcon.png", width, height, -1);

	toolbarPlayTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ToolbarPlayIcon.png", width, height, -1);
	toolbarPauseTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ToolbarPauseIcon.png", width, height, -1);
	toolbarStepTexID = App->textures->CreateTextureFromFile("Settings/EditorResources/ToolbarStepIcon.png", width, height, -1);

	// REMEMBER to gldeletetex them at cleanup!
}

be_imguialloc ModuleGui::GetImGuiAlloc() const {
	return &ImGuiCustomAllocator;
}

be_imguifree ModuleGui::GetImGuiFree() const {
	return &ImGuiCustomDeallocator;
}
