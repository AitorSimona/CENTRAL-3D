#include "PanelAbout.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleGui.h"

#include "Imgui/imgui.h"

PanelAbout::PanelAbout(char * name) : Panel(name)
{
}

PanelAbout::~PanelAbout()
{
}

bool PanelAbout::Draw() 
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		// --- Introduction ---
		ImGui::Separator();
		ImGui::Text(EngineApp->GetAppName());
		ImGui::SameLine();
		ImGui::Text("Version 0.0");
		ImGui::SameLine();
		if (ImGui::Button("GitHub")) { EngineApp->gui->RequestBrowser("https://github.com/Broken-Gem-Studio/Broken-Engine"); }
		ImGui::Text("A 3D Game Engine developed by students of CITM-Barcelona for 3rd year's project 3 subject");
		ImGui::Text("By: ");
		ImGui::SameLine();
		if (ImGui::Button("Broken Gem Studio")) { EngineApp->gui->RequestBrowser("https://github.com/Broken-Gem-Studio"); }

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// --- License ---

		ImGui::TextWrapped("MIT License");
		ImGui::TextWrapped("Copyright(c) 2019 Aitor Simona Bouzas");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files(the <Software>), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions : ");
		ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
		ImGui::TextWrapped("THE SOFTWARE IS PROVIDED <AS IS>, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

		ImGui::Spacing();

		ImGui::TextWrapped("PhysX License");
		ImGui::TextWrapped("Copyright (c) 2018-2019 NVIDIA Corporation. All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:");
		ImGui::TextWrapped("- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.");
		ImGui::TextWrapped("- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.");
		ImGui::TextWrapped("- Neither the name of NVIDIA CORPORATION nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.");
		ImGui::TextWrapped("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");
		
	}
	ImGui::End();



	return true;
}
