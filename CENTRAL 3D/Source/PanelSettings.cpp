#include <vector>
#include "PanelSettings.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleHardware.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleTimeManager.h"


#include "Imgui/imgui.h"
#include "OpenGL.h"
#include "DevIL/include/il.h"

#include "mmgr/mmgr.h"


PanelSettings::PanelSettings(char * name): Panel(name) , FPS_Tracker(FPS_TRACKER_SIZE), MS_Tracker(FPS_TRACKER_SIZE)
{

}

PanelSettings::~PanelSettings() 
{

}

bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			ApplicationNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Window"))
		{
			WindowNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Input"))
		{
			InputNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Renderer"))
		{
			RendererNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			HardwareNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Libraries"))
		{
			LibrariesNode();
			ImGui::Separator();
		}
	}
	ImGui::End();


	return true;
}

inline void PanelSettings::ApplicationNode() const
{
	// --- Application name ---
	static char appName[100];
	if (App->GetAppName() != nullptr)
	   strcpy_s(appName, 100, App->GetAppName());
	if (ImGui::InputText("App Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		App->SetAppName(appName);
	
	ImGui::Separator();

	// --- Organization name ---
	static char orgName[100];
	if (App->GetOrganizationName() != nullptr)
		strcpy_s(orgName, 100, App->GetOrganizationName());
	if (ImGui::InputText("Organization Name", orgName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		App->SetOrganizationName(orgName);

	ImGui::Separator();
	// --- Cap frames ---
	int maxFramerate = App->time->GetMaxFramerate();
	if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, App->window->GetDisplayRefreshRate()))
		App->time->SetMaxFramerate(maxFramerate);

	ImGui::Separator();

	ImGui::Text("Limit Framerate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255,255,0,255), "%i", App->time->GetMaxFramerate());

	// --- Framerate && Ms ---
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", FPS_Tracker[FPS_Tracker.size() - 1]);
	ImGui::PlotHistogram("##Framerate", &FPS_Tracker[0], FPS_Tracker.size(), 0, title, 0.0f, 100.0f, ImVec2(500, 75));
	sprintf_s(title, 25, "Milliseconds %0.1f", MS_Tracker[MS_Tracker.size() - 1]);
	ImGui::PlotHistogram("##Milliseconds", &MS_Tracker[0], MS_Tracker.size(), 0, title, 0.0f, 40.0f, ImVec2(500, 75));

	// --- Memory ---
	sMStats MemoryStats = m_getMemoryStatistics(); // Using mmgr 
	static int speed = 0;
	static std::vector<float> Memory(100); // Hom many units/lines we want in the plot
	if (++speed > 25) // How fast the plot is plotted :)
	{
		speed = 0;
		if (Memory.size() == 100)
		{
			for (uint i = 0; i < 100 - 1; ++i)
				Memory[i] = Memory[i + 1];

			Memory[100 - 1] = (float)MemoryStats.totalReportedMemory;
		}
		else
			Memory.push_back((float)MemoryStats.totalReportedMemory);
	}

	ImGui::PlotHistogram("##Memory", &Memory[0], Memory.size(), 0, "Memory Consumption", 0.0f, (float)MemoryStats.peakReportedMemory * 1.2f, ImVec2(500, 75));

	ImGui::Text("Total Reported Memory: %u", MemoryStats.totalReportedMemory);
	ImGui::Text("Total Actual Memory: %u", MemoryStats.totalActualMemory);
	ImGui::Text("Peak Reported Memory: %u", MemoryStats.peakReportedMemory);
	ImGui::Text("Peak Actual Memory: %u", MemoryStats.peakActualMemory);
	ImGui::Text("Accumulated Reported Memory: %u", MemoryStats.accumulatedReportedMemory);
	ImGui::Text("Accumulated Actual Memory: %u", MemoryStats.accumulatedActualMemory);
	ImGui::Text("Accumulated Alloc Unit Count: %u", MemoryStats.accumulatedAllocUnitCount);
	ImGui::Text("Total Alloc Unit Count: %u", MemoryStats.totalAllocUnitCount);
	ImGui::Text("Peak Alloc Unit Count: %u", MemoryStats.peakAllocUnitCount);
}

inline void PanelSettings::WindowNode() const
{
	ImGui::Separator();
	// --- Brightness ---
	float brightness = App->window->GetWinBrightness();
	if (ImGui::SliderFloat("Window Brightness", &brightness, 0.0f, 1.0f))
		App->window->SetWinBrightness(brightness);

	// --- Window ReSize ---
	uint width, height, min_width, min_height, max_width, max_height;
	App->window->GetWinMaxMinSize(min_width, min_height, max_width, max_height);
	width = App->window->GetWindowWidth();
	height = App->window->GetWindowHeight();

	if (ImGui::SliderInt("Width", (int*)&width, min_width, max_width))
		App->window->SetWindowWidth(width);
	if (ImGui::SliderInt("Height", (int*)&height, min_height, max_height))
		App->window->SetWindowHeight(height);

	ImGui::Separator();

	// --- Refresh Rate Indicator ---
	ImGui::Text("Refresh Rate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%u", App->window->GetDisplayRefreshRate());


	// --- Window Flags ---
	bool fullscreen = App->window->IsFullscreen();
	bool fullscreen_desktop = App->window->IsFullscreenDesktop();
	bool resizable = App->window->IsResizable();
	bool borderless = App->window->IsBorderless();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);

		ImGui::SameLine();

	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Restart to apply"); // Cannot Change Resizable flag at runtime

	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);

		ImGui::SameLine();

	if (ImGui::Checkbox("FullScreen Desktop", &fullscreen_desktop))
		App->window->SetFullscreenDesktop(fullscreen_desktop);
}

inline void PanelSettings::InputNode() const
{
	ImGui::Separator();
	// --- Mouse position
	int mouse_x, mouse_y;
	mouse_x = App->input->GetMouseX();
	mouse_y = App->input->GetMouseY();
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i,%i", mouse_x, mouse_y);

	// --- Mouse Motion ---
	mouse_x = App->input->GetMouseXMotion();
	mouse_y = App->input->GetMouseYMotion();
	ImGui::Text("Mouse Motion:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i,%i", mouse_x, mouse_y);

	// --- Mouse Wheel acc ---
	int wheel = App->input->GetMouseWheel();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", wheel);

	ImGui::Separator();
}

inline void PanelSettings::RendererNode() const
{
	// --- VSync ---
	bool vsync = App->renderer3D->GetVSync();
	if (ImGui::Checkbox("VSync", &vsync))
	{
		App->renderer3D->SetVSync(vsync);
	}
	if (ImGui::Checkbox("DEPTH", &App->renderer3D->depth))
	{ }

	if (ImGui::Checkbox("FACE CULLING", &App->renderer3D->cull_face))
	{ }

}


inline void PanelSettings::HardwareNode() const
{
	hw_info hardware_info = App->hardware->GetInfo();

	ImGui::Separator();
	// --- SDL Version ---
	ImGui::Text("SDL Version:");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.sdl_version); 
	ImGui::Separator();

	// --- CPU ---
	ImGui::Text("CPU Logic Cores:");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.cpu_count);

	ImGui::Text("CPU L1 Cache (Kb):");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.l1_cachekb);

	ImGui::Text("CPU Instruction Support:");ImGui::SameLine();

	if(hardware_info.rdtsc)
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "rdtsc");ImGui::SameLine();
	if (hardware_info.altivec)
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "altivec");ImGui::SameLine();
	if (hardware_info.now3d)
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "now3d");ImGui::SameLine();
	if (hardware_info.mmx)							   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "mmx");	ImGui::SameLine();
	if (hardware_info.sse)
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse");	ImGui::SameLine();
	if (hardware_info.sse2)							   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse2");ImGui::SameLine();
	if (hardware_info.sse3)							   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse3");ImGui::SameLine();
	if (hardware_info.sse41)						   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse41"); ImGui::SameLine();
	if (hardware_info.sse42)						   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse42");ImGui::SameLine();
	if (hardware_info.avx)							   
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "avx");	ImGui::SameLine();
	if (hardware_info.avx2)						
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "avx2");

	ImGui::Separator();
	// --- RAM ---
	ImGui::Text("RAM Memory (Gb)");ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.ram_gb);

	ImGui::Separator();

	// --- GPU --- 
	ImGui::Text("GPU Vendor");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_vendor.data());

	ImGui::Text("GPU Model"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_brand.data());

	ImGui::Text("GPU Driver"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_driver.data());

	// (Currently NVIDIA only)
	ImGui::Text("VRAM Budget");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_budget);

	ImGui::Text("VRAM Available"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_available);

	ImGui::Text("VRAM Usage"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_usage);
}

inline void PanelSettings::LibrariesNode() const
{
	// --- Libraries ---
	ImGui::Spacing();

	SDL_version version;
	SDL_GetVersion(&version);
	if (ImGui::Button("SDL")) { App->gui->RequestBrowser("https://www.libsdl.org/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i.%i.%i", version.major, version.minor, version.patch);

	if (ImGui::Button("OpenGL")) { App->gui->RequestBrowser("https://www.opengl.org/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", glGetString(GL_VERSION));

	if (ImGui::Button("Glew")) { App->gui->RequestBrowser("http://glew.sourceforge.net/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", glewGetString(GLEW_VERSION));

	if (ImGui::Button("ImGui")) { App->gui->RequestBrowser("https://github.com/ocornut/imgui"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", ImGui::GetVersion());

	if (ImGui::Button("ImGuizmo")) { App->gui->RequestBrowser("https://github.com/CedricGuillemet/ImGuizmo"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "1.0");

	if (ImGui::Button("MathGeoLib")) { App->gui->RequestBrowser("https://github.com/juj/MathGeoLib"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "1.5");

	if (ImGui::Button("JSON For Modern C++")) { App->gui->RequestBrowser("https://github.com/nlohmann/json"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "3.7.0");

	if (ImGui::Button("DevIL")) { App->gui->RequestBrowser("http://openil.sourceforge.net/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", IL_VERSION);

	if (ImGui::Button("Par shapes")) { App->gui->RequestBrowser("https://github.com/prideout/par/blob/master/par_shapes.h"); }

	if (ImGui::Button("MMGR")) { App->gui->RequestBrowser("http://www.paulnettle.com/"); }

}

void PanelSettings::AddFPS(float fps, float ms)
{
	static uint counter = 0;

	// --- If the plot historiogram is already full we shift ---
	if (counter == FPS_TRACKER_SIZE)
	{
		for (uint i = 0; i < FPS_TRACKER_SIZE - 1; ++i)
		{
			FPS_Tracker[i] = FPS_Tracker[i + 1];
			MS_Tracker[i] = MS_Tracker[i + 1];
		}
	}
	else // We count until the max size is reached, then substitute values in the if above
		++counter;

	FPS_Tracker[counter - 1] = fps;
	MS_Tracker[counter - 1] = ms;
}

