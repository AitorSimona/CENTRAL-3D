#include "PanelSettings.h"
#include "Application.h"
#include "Source/Imgui/imgui.h"
#include "Source/mmgr/mmgr.h"
#include <vector>

PanelSettings::PanelSettings(char * name): Panel(name) , fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{

}

PanelSettings::~PanelSettings() 
{

}

bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::TreeNode("Application"))
		{
			ApplicationNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Window"))
		{
			WindowNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Input"))
		{
			InputNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hardware"))
		{
			HardwareNode();
			ImGui::TreePop();
		}
	}
	ImGui::End();


	return true;
}

void PanelSettings::ApplicationNode() const
{
	// --- Application name ---
	static char appName[20] = "CENTRAL 3D";
	ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName));

	// --- Cap frames ---
	int maxFramerate = App->GetMaxFramerate();
	if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, App->window->GetDisplayRefreshRate()))
		App->SetMaxFramerate(maxFramerate);

	ImGui::Text("Limit Framerate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255,255,0,255), "%i", App->GetMaxFramerate());
	
	// --- VSync ---
	bool vsync = App->renderer3D->GetVSync();
	if (ImGui::Checkbox("Use VSync", &vsync))
	{
		App->renderer3D->SetVSync(vsync);
	}

	// --- Framerate && Ms ---
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	// --- Memory ---
	sMStats stats = m_getMemoryStatistics();
	static int speed = 0;
	static std::vector<float> memory(100);
	if (++speed > 20)
	{
		speed = 0;
		if (memory.size() == 100)
		{
			for (uint i = 0; i < 100 - 1; ++i)
				memory[i] = memory[i + 1];

			memory[100 - 1] = (float)stats.totalReportedMemory;
		}
		else
			memory.push_back((float)stats.totalReportedMemory);
	}

	ImGui::PlotHistogram("##memory", &memory[0], memory.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
	ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
	ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
	ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
	ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
	ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
	ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
	ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
	ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
}

void PanelSettings::WindowNode() const
{
}

void PanelSettings::InputNode() const
{
}

void PanelSettings::HardwareNode() const
{
}

void PanelSettings::AddFPS(float fps, float ms)
{
	static uint count = 0;

	if (count == FPS_LOG_SIZE)
	{
		for (uint i = 0; i < FPS_LOG_SIZE - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}

