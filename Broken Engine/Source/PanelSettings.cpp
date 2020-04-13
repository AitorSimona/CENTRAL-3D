#include "PanelSettings.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleGui.h"
#include "ModuleTimeManager.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleHardware.h"

// -- Components --
#include "ComponentCamera.h"

// -- Utilities --
#include "Imgui/imgui.h"
#include "OpenGL.h"
#include "DevIL/include/il.h"
#include "Assimp/include/version.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")
#include "mmgr/mmgr.h"


PanelSettings::PanelSettings(char * name): Panel(name) , FPS_Tracker(FPS_TRACKER_SIZE), MS_Tracker(FPS_TRACKER_SIZE)
{

}

PanelSettings::~PanelSettings() 
{

}

bool PanelSettings::Draw()
{
	//We get the info for the last frame
	float fps = EngineApp->time->GetFPS();
	float last_frame_ms = EngineApp->time->GetLastFrameMs();
	if (fps != -1)
		AddFPS(fps, last_frame_ms);

	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

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
		if (ImGui::CollapsingHeader("Camera Settings"))
		{
			EngineCameraNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			HardwareNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Software"))
		{
			SoftwareNode();
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Memory"))
		{
			RAMMemoryNode();
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
	if (EngineApp->GetAppName() != nullptr)
	   strcpy_s(appName, 100, EngineApp->GetAppName());
	if (ImGui::InputText("App Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		EngineApp->SetAppName(appName);
	
	ImGui::Separator();

	// --- Organization name ---
	static char orgName[100];
	if (EngineApp->GetOrganizationName() != nullptr)
		strcpy_s(orgName, 100, EngineApp->GetOrganizationName());
	if (ImGui::InputText("Organization Name", orgName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		EngineApp->SetOrganizationName(orgName);

	ImGui::Separator();
	// --- Cap frames ---
	int maxFramerate = EngineApp->time->GetMaxFramerate();
	if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, EngineApp->window->GetDisplayRefreshRate()))
		EngineApp->time->SetMaxFramerate(maxFramerate);

	ImGui::Separator();

	ImGui::Text("Limit Framerate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255,255,0,255), "%i", EngineApp->time->GetMaxFramerate());

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
	float brightness = EngineApp->window->GetWinBrightness();
	if (ImGui::SliderFloat("Window Brightness", &brightness, 0.0f, 1.0f))
		EngineApp->window->SetWinBrightness(brightness);

	// --- Window ReSize ---
	uint width, height, min_width, min_height, max_width, max_height;
	EngineApp->window->GetWinMaxMinSize(min_width, min_height, max_width, max_height);
	width = EngineApp->window->GetWindowWidth();
	height = EngineApp->window->GetWindowHeight();

	if (ImGui::SliderInt("Width", (int*)&width, min_width, max_width))
		EngineApp->window->SetWindowWidth(width);
	if (ImGui::SliderInt("Height", (int*)&height, min_height, max_height))
		EngineApp->window->SetWindowHeight(height);

	ImGui::Separator();

	// --- Refresh Rate Indicator ---
	ImGui::Text("Refresh Rate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%u", EngineApp->window->GetDisplayRefreshRate());


	// --- Window Flags ---
	bool fullscreen = EngineApp->window->IsFullscreen();
	bool fullscreen_desktop = EngineApp->window->IsFullscreenDesktop();
	bool resizable = EngineApp->window->IsResizable();
	bool borderless = EngineApp->window->IsBorderless();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		EngineApp->window->SetFullscreen(fullscreen);

		ImGui::SameLine();

	if (ImGui::Checkbox("Resizable", &resizable))
		EngineApp->window->SetResizable(resizable);

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Restart to apply"); // Cannot Change Resizable flag at runtime

	if (ImGui::Checkbox("Borderless", &borderless))
		EngineApp->window->SetBorderless(borderless);

		ImGui::SameLine();

	if (ImGui::Checkbox("FullScreen Desktop", &fullscreen_desktop))
		EngineApp->window->SetFullscreenDesktop(fullscreen_desktop);
}

inline void PanelSettings::InputNode() const
{
	ImGui::Separator();
	// --- Mouse position
	int mouse_x, mouse_y;
	mouse_x = EngineApp->input->GetMouseX();
	mouse_y = EngineApp->input->GetMouseY();
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i,%i", mouse_x, mouse_y);

	// --- Mouse Motion ---
	mouse_x = EngineApp->input->GetMouseXMotion();
	mouse_y = EngineApp->input->GetMouseYMotion();
	ImGui::Text("Mouse Motion:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i,%i", mouse_x, mouse_y);

	// --- Mouse Wheel acc ---
	int wheel = EngineApp->input->GetMouseWheel();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", wheel);

	ImGui::Separator();
}

inline void PanelSettings::RendererNode() const
{
	// --- VSync ---
	bool vsync = EngineApp->renderer3D->GetVSync();
	if (ImGui::Checkbox("VSync", &vsync))
	{
		EngineApp->renderer3D->SetVSync(vsync);
	}
	if (ImGui::Checkbox("DEPTH", &EngineApp->renderer3D->depth))
	{ }

	if (ImGui::Checkbox("FACE CULLING", &EngineApp->renderer3D->cull_face))
	{ }

}

inline void PanelSettings::EngineCameraNode() const
{
	Broken::ComponentCamera* cam = EngineApp->camera->camera;

	// --- Camera Speed ---
	float camSpeed = EngineApp->camera->m_CameraSpeed;
	float camScroll = EngineApp->camera->m_ScrollSpeed;

	ImGui::Text("Camera Speed");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);	
	ImGui::DragFloat("##CamSpeed", &camSpeed, 0.05f, 1.0f, 30.0f);
	
	ImGui::Text("Camera Scroll Speed");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##CamScroll", &camScroll, 0.05f, 1.0f, 10.0f);

	if (camSpeed != EngineApp->camera->m_CameraSpeed)
		EngineApp->camera->m_CameraSpeed = camSpeed;
	if (camScroll != EngineApp->camera->m_ScrollSpeed)
		EngineApp->camera->m_ScrollSpeed = camScroll;

	// --- Camera FOV ---
	ImGui::Text("FOV");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	float fov = cam->GetFOV();
	ImGui::DragFloat("##FOV", &fov, 0.005f, 0.005f, 179.0f);

	if (fov != cam->GetFOV())
		EngineApp->camera->camera->SetFOV(fov);

	ImGui::SameLine();
	if (ImGui::Button("DefFOV", { 50.0f, 15.0f }))
		EngineApp->camera->m_CustomDefaultCameraValues.x = fov;

	// --- Camera Planes ---
	float nearPlane = cam->GetNearPlane();
	float farPlane = cam->GetFarPlane();

	ImGui::Text("Camera Planes");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##NearPlane", &nearPlane, 0.005f, 0.01f, farPlane - 0.01f);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##FarPlane", &farPlane, 0.005f, nearPlane + 0.01f, 10000.0f);


	if (nearPlane != cam->GetNearPlane())
		EngineApp->camera->camera->SetNearPlane(nearPlane);
	if (farPlane != cam->GetFarPlane())
		EngineApp->camera->camera->SetFarPlane(farPlane);

	ImGui::SameLine();
	if (ImGui::Button("DefPlanes", { 75.0f, 15.0f }))
	{
		EngineApp->camera->m_CustomDefaultCameraValues.y = nearPlane;
		EngineApp->camera->m_CustomDefaultCameraValues.z = farPlane;
	}

	// --- Camera Aspect Ratio ---
	float aspectRatio = cam->GetAspectRatio();

	ImGui::Text("Aspect Ratio");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::DragFloat("##AspectRatio", &aspectRatio, 0.005f, 1.0f, 4.0f);

	if (aspectRatio != cam->GetAspectRatio())
		EngineApp->camera->camera->SetAspectRatio(aspectRatio);

	ImGui::SameLine();
	if (ImGui::Button("DefAR", { 50.0f, 15.0f }))
		EngineApp->camera->m_CustomDefaultCameraValues.w = aspectRatio;

	// --- Set Values to Default ---
	if (ImGui::Button("Custom Default Values", { 150.0f, 25.0f }))
	{
		EngineApp->camera->camera->SetCameraValues(EngineApp->camera->m_CustomDefaultCameraValues);
		EngineApp->camera->m_CameraSpeed = 10.0f;
		EngineApp->camera->m_ScrollSpeed = 3.0f;
	}

	ImGui::SameLine();
	if (ImGui::Button("Default Values", { 150.0f, 25.0f }))
	{
		EngineApp->camera->camera->SetCameraValues(EngineApp->camera->GetCameraDefaultValues());
		EngineApp->camera->m_CameraSpeed = 10.0f;
		EngineApp->camera->m_ScrollSpeed = 3.0f;
	}
}

inline void PanelSettings::SoftwareNode() const
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
	ImVec4 Color = ImVec4(255, 255, 0, 255);	
	Broken::SoftwareInfo swInfo = EngineApp->hardware->GetSwInfo();

	// --- INFO
	ImGui::Text("Current Compiled Date: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetCompilationDate());
	ImGui::SameLine(); ImGui::Text("and Time: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetCompilationTime());
	ImGui::Text("Multithreaded specified: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.MultithreadedSpecified());

	ImGui::Separator();
	ImGui::Text("Windows OS: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetWindowsVersion());
	ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetSDLVersion());
	ImGui::Text("OpenGL Version: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetOGLVersion());
	ImGui::SameLine(); ImGui::Text("   OpenGL Shading Version: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetOGLShadingVersion());

	ImGui::Separator();
	ImGui::Text("C++ Minimum Version Supported by Compiler: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetCppVersionImplementedByCompiler());
	ImGui::SameLine(); ImGui::Text(" ("); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetCPPNumericalVersion()); ImGui::SameLine(); ImGui::Text(")");

	ImGui::Text("C++ Used Version: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", (swInfo.GetCppCompilerVersion()));
	ImGui::Text("Visual Studio Compiler Version: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", swInfo.GetVSCompilerVersion());
	// ---

	ImGui::PopStyleVar();
}


inline void PanelSettings::RAMMemoryNode() const
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
	ImVec4 Color = ImVec4(255, 255, 0, 255);
	Broken::MemoryHardware HardwareInfo = EngineApp->hardware->GetMemInfo();

	// --- INFO
	ImGui::Text("Total System RAM: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetRAMSizeFromSDL()) + " GB").c_str()));
	ImGui::Text("Total Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetPhysicalMemory()) + " GB").c_str()));
	ImGui::Text("Available Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetFreePhysicalMemory()) + " GB").c_str()));
	ImGui::Text("Used Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetUsedPhysicalMemory()) + " GB").c_str()));
	ImGui::Text("Percentage of Memory Load: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetPercentageOfMemoryLoad()) + " " + "%%").c_str()));
	ImGui::Text("");
	ImGui::Text("Physical Memory Used by Process: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetPhysMemoryUsedByProcess()) + " MB").c_str()));
	ImGui::Text("Virtual Memory Used by Process: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetVirtualMemoryUsedByProcess()) + " MB").c_str()));

	ImGui::Text(" MMGR Memory Statistics"); ImGui::SameLine(); ImGui::Separator();
	ImGui::Text("Total Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_TotalReportedMemory()) + " Bytes").c_str()));
	ImGui::Text("Total Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_TotalActualMemory()) + " Bytes").c_str()));
	ImGui::Text("Peak Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_PeakReportedMemory()) + " Bytes").c_str()));
	ImGui::Text("Peak Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_PeakActualMemory()) + " Bytes").c_str()));
	ImGui::Text("Accumulated Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_AccumulatedReportedMemory())).c_str()));
	ImGui::Text("Accumulated Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", ((std::to_string(HardwareInfo.GetMemStatsFromMMGR_AccumulatedActualMemory())).c_str()));

	ImGui::Text(" MMGR Allocated Unit Count Statistics"); ImGui::SameLine(); ImGui::Separator();
	ImGui::Text("Total Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", (std::to_string(HardwareInfo.GetMemStatsFromMMGR_TotalAllocUnitCount()).c_str()));
	ImGui::Text("Peak Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", (std::to_string(HardwareInfo.GetMemStatsFromMMGR_PeakAllocUnitCount()).c_str()));
	ImGui::Text("Accumulated Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, "%s", (std::to_string(HardwareInfo.GetMemStatsFromMMGR_AccumulatedAllocUnitCount()).c_str()));
	// ---

	ImGui::PopStyleVar();
}

inline void PanelSettings::HardwareNode() const
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
	ImVec4 Color = ImVec4(255, 255, 0, 255);
	const Broken::ProcessorHardware& CPUData = EngineApp->hardware->GetProcessorInfo();
	const Broken::GPUHardware& GPUData = EngineApp->hardware->GetGraphicsCardInfo();

	// --- INFO
	ImGui::Text("PROCESSOR INFORMATION (CPU)");
	ImGui::Separator();

	ImGui::Text("CPU Brand: "); ImGui::SameLine(); ImGui::TextColored(Color, (CPUData.GetCPUBrand()));
	ImGui::Text("CPU Vendor: "); ImGui::SameLine(); ImGui::TextColored(Color, (CPUData.GetCPUVendor()));
	ImGui::Text("CPU Arhitecture: "); ImGui::SameLine(); ImGui::TextColored(Color, (CPUData.GetCPUArchitecture()));
	ImGui::SameLine(); ImGui::Text("    CPU Cores: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(CPUData.GetCPUCores()).c_str());
	ImGui::SameLine(); ImGui::Text("    CPU Processors: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(CPUData.GetNumberOfProcessors()).c_str());
	ImGui::Text("");
	ImGui::Text("CPU Revision: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(CPUData.GetProcessorRevision()).c_str());
	ImGui::Text("CPU Line L1 Cache Size: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(CPUData.GetCPUCacheLine1Size()) + " Bytes").c_str());
	ImGui::Text("");
	ImGui::Text("CPU Instructions Set: "); ImGui::SameLine(); ImGui::TextColored(Color, CPUData.GetCPUInstructionSet());
	// ---

	ImGui::Separator();
	ImGui::Text("GRAPHICS CARD INFORMATION (GPU)");
	ImGui::Separator();

	ImGui::Text("GPU Benchmark: "); ImGui::SameLine(); ImGui::TextColored(Color, GPUData.GetGPUBenchmark());
	ImGui::Text("GPU Brand: "); ImGui::SameLine(); ImGui::TextColored(Color, GPUData.GetGPUInfo_GPUDet().m_GPUBrand.c_str());
	ImGui::Text("GPU Model: "); ImGui::SameLine(); ImGui::TextColored(Color, GPUData.GetGPUModel());
	ImGui::Text("GPU Driver"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", EngineApp->hardware->GetInfo().gpu_driver.data());
	ImGui::Text("GPU Vendor: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(GPUData.GetGPUInfo_GPUDet().m_GPUVendor).c_str());
	ImGui::SameLine(); ImGui::Text("   GPU ID: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(GPUData.GetGPUInfo_GPUDet().m_GPUID).c_str());

	// (Currently NVIDIA only)
	ImGui::Text("GPU VRAM"); ImGui::SameLine(); ImGui::Separator();

	ImGui::Text("Total VRAM: ");	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", (std::to_string(EngineApp->hardware->GetInfo().vram_mb_budget) + " GB").c_str());
	ImGui::Text("VRAM Without System Usage: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(GPUData.GetGPUInfo_GPUDet().mPI_GPUDet_TotalVRAM_MB) + " GB").c_str());
	ImGui::Text("VRAM Available"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", (std::to_string(EngineApp->hardware->GetInfo().vram_mb_available) + " GB").c_str());
	ImGui::Text("Actual/Real VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(GPUData.GetGPUInfo_GPUDet().mPI_GPUDet_CurrentVRAM_MB) + " GB").c_str());
	ImGui::Text("Reserved VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(GPUData.GetGPUInfo_GPUDet().mPI_GPUDet_VRAMReserved_MB) + " GB").c_str());
	ImGui::Text("Used VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(GPUData.GetGPUInfo_GPUDet().mPI_GPUDet_VRAMUsage_MB) + " GB").c_str());


	if (ImGui::Button("Recalculate Parameters"))
		EngineApp->hardware->RecalculateParameters();

	ImGui::PopStyleVar();
}

//inline void PanelSettings::HardwareNode() const
//{
//	hw_info hardware_info = EngineApp->hardware->GetInfo();
//
//	// --- CPU ---
//	ImGui::Separator();
//	ImGui::Text("PROCESSOR INFORMATION (CPU)");
//	
//	ImGui::Text("CPU Logic Cores:");	ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.cpu_count);
//
//	ImGui::Text("CPU L1 Cache (Kb):");	ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.l1_cachekb);
//
//	ImGui::Text("CPU Instruction Support:");ImGui::SameLine();
//
//	if(hardware_info.rdtsc)
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "RDTSC");ImGui::SameLine();
//	if (hardware_info.altivec)
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "ALTIVEC");ImGui::SameLine();
//	if (hardware_info.now3d)
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "NOW3D");ImGui::SameLine();
//	if (hardware_info.mmx)							   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "MMX");	ImGui::SameLine();
//	if (hardware_info.sse)
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "SSE");	ImGui::SameLine();
//	if (hardware_info.sse2)							   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "SSE2");ImGui::SameLine();
//	if (hardware_info.sse3)							   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "SSE3");ImGui::SameLine();
//	if (hardware_info.sse41)						   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "SSE41"); ImGui::SameLine();
//	if (hardware_info.sse42)						   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "SSE42");ImGui::SameLine();
//	if (hardware_info.avx)							   
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "AVX");	ImGui::SameLine();
//	if (hardware_info.avx2)						
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "AVX2");
//
//	// --- GPU --- 
//	ImGui::Separator();
//	ImGui::Text("GRAPHICS CARD INFORMATION (GPU)");
//
//	ImGui::Text("GPU Vendor");	ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_vendor.data());
//
//	ImGui::Text("GPU Model"); ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_brand.data());
//
//	ImGui::Text("GPU Driver"); ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_driver.data());
//
//	// (Currently NVIDIA only)
//	ImGui::Text("VRAM Budget");	ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_budget);
//
//	ImGui::Text("VRAM Available"); ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_available);
//
//	ImGui::Text("VRAM Usage"); ImGui::SameLine();
//	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_usage);
//}

inline void PanelSettings::LibrariesNode() const
{
	// --- Libraries ---
	ImGui::Spacing();

	SDL_version version;
	SDL_GetVersion(&version);
	if (ImGui::Button("SDL")) { EngineApp->gui->RequestBrowser("https://www.libsdl.org/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i.%i.%i", version.major, version.minor, version.patch);

	if (ImGui::Button("Assimp")) { EngineApp->gui->RequestBrowser("http://www.assimp.org/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i.%i", aiGetVersionMajor(), aiGetVersionMinor());

	if (ImGui::Button("OpenGL")) { EngineApp->gui->RequestBrowser("https://www.opengl.org/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "4.4"/*glGetString(GL_VERSION)*/);

	if (ImGui::Button("Glad")) { EngineApp->gui->RequestBrowser("https://github.com/Dav1dde/glad"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "0.1.33");

	if (ImGui::Button("ImGui")) { EngineApp->gui->RequestBrowser("https://github.com/ocornut/imgui"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", ImGui::GetVersion());

	if (ImGui::Button("ImGuizmo")) { EngineApp->gui->RequestBrowser("https://github.com/CedricGuillemet/ImGuizmo"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "1.0");

	if (ImGui::Button("MathGeoLib")) { EngineApp->gui->RequestBrowser("https://github.com/juj/MathGeoLib"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "1.5");

	if (ImGui::Button("JSON For Modern C++")) { EngineApp->gui->RequestBrowser("https://github.com/nlohmann/json"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "3.7.0");

	if (ImGui::Button("DevIL")) { EngineApp->gui->RequestBrowser("http://openil.sourceforge.net/"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", IL_VERSION);

	if (ImGui::Button("Par shapes")) { EngineApp->gui->RequestBrowser("https://github.com/prideout/par/blob/master/par_shapes.h"); }

	if (ImGui::Button("MMGR")) { EngineApp->gui->RequestBrowser("http://www.paulnettle.com/"); }

	if (ImGui::Button("PhysX")) { EngineApp->gui->RequestBrowser("https://github.com/NVIDIAGameWorks/PhysX-3.4/releases/tag/v3.4.2-bsd"); }
	ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "3.4.2");

}

void PanelSettings::AddFPS(const float& fps, const float& ms)
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

