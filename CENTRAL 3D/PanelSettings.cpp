#include "PanelSettings.h"
#include "Application.h"
#include "Source/Imgui/imgui.h"
#include "Source/mmgr/mmgr.h"
#include <vector>

PanelSettings::PanelSettings(char * name): Panel(name) {}

PanelSettings::~PanelSettings() {}

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
	
	bool capFrames = App->GetCapFrames();
	if (ImGui::Checkbox("Cap Frames", &capFrames))
	{
		App->SetCapFrames(capFrames);
		if (capFrames && App->renderer3D->GetVSync())
			App->renderer3D->SetVSync(false);
	}

	if (capFrames)
	{
		int maxFramerate = App->GetMaxFramerate();
		if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, 144))
			App->SetMaxFramerate(maxFramerate);
	}

	// --- VSync ---
	bool vsync = App->renderer3D->GetVSync();
	if (ImGui::Checkbox("Use VSync", &vsync))
	{
		App->renderer3D->SetVSync(vsync);
		if (vsync && App->GetCapFrames())
			App->SetCapFrames(false);
	}

	// --- Framerate ---
	char title[20];
	std::vector<float> framerateTrack = App->GetFramerateTrack();
	sprintf_s(title, IM_ARRAYSIZE(title), "Framerate %.1f", framerateTrack.back());
	ImGui::PlotHistogram("##framerate", &framerateTrack.front(), framerateTrack.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	// --- Ms ---
	std::vector<float> msTrack = App->GetMsTrack();
	sprintf_s(title, IM_ARRAYSIZE(title), "Milliseconds %.1f", msTrack.back());
	ImGui::PlotHistogram("##milliseconds", &msTrack.front(), msTrack.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	// --- Memory ---
	sMStats memStats = m_getMemoryStatistics();

	static std::vector<float> memory(100);

	for (uint i = memory.size() - 1; i > 0; --i)
		memory[i] = memory[i - 1];

	memory[0] = (float)memStats.peakActualMemory;

	ImGui::PlotHistogram("##RAMusage", &memory.front(), memory.size(), 0, "Ram Usage", 0.0f, (float)memStats.peakReportedMemory * 1.2f, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem: %u", memStats.totalReportedMemory);
	ImGui::Text("Total Actual Mem: %u", memStats.totalActualMemory);
	ImGui::Text("Peak Reported Mem: %u", memStats.peakReportedMemory);
	ImGui::Text("Peak Actual Mem: %u", memStats.peakActualMemory);
	ImGui::Text("Accumulated Reported Mem: %u", memStats.accumulatedReportedMemory);
	ImGui::Text("Accumulated Actual Mem: %u", memStats.accumulatedActualMemory);
	ImGui::Text("Acumulated Alloc Unit Count: %u", memStats.accumulatedAllocUnitCount);
	ImGui::Text("Total Alloc Unit Count: %u", memStats.totalAllocUnitCount);
	ImGui::Text("Peak Alloc Unit Count: %u", memStats.peakAllocUnitCount);
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

