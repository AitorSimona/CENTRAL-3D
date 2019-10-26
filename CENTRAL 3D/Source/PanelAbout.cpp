#include "PanelAbout.h"
#include "Application.h"
#include "ModuleGui.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"


PanelAbout::PanelAbout(char * name) : Panel(name)
{
}

PanelAbout::~PanelAbout()
{
}

bool PanelAbout::Draw() 
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		// --- Introduction ---
		ImGui::Separator();
		ImGui::Text("CENTRAL 3D");
		ImGui::SameLine();
		ImGui::Text("Version 0.05");
		ImGui::SameLine();
		if (ImGui::Button("GitHub")) { App->gui->RequestBrowser("https://github.com/AitorSimona/CENTRAL-3D"); }
		ImGui::Text("Initial steps into game engine development");
		ImGui::Text("By: ");
		ImGui::SameLine();
		if (ImGui::Button("Aitor Simona")) { App->gui->RequestBrowser("https://github.com/AitorSimona"); }

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// --- License ---

		ImGui::TextWrapped("MIT License");
		ImGui::TextWrapped("Copyright(c) 2019 Aitor Simona Bouzas");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files(the <Software>), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions : ");
		ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
		ImGui::TextWrapped("THE SOFTWARE IS PROVIDED <AS IS>, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	}

	ImGui::End();

	return true;
}
