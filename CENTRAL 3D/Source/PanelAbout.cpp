#include "PanelAbout.h"
#include "Application.h"
#include "ModuleGui.h"
#include "Imgui/imgui.h"
#include "SDL/include/SDL.h"
#include "DevIL/include/il.h"

#include "OpenGL.h"

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
		ImGui::Text("Version 0 ");
		ImGui::Text("Initial steps into game engine development");
		ImGui::Text("By Aitor Simona");

		// --- Libraries ---
		ImGui::Text("Libraries");
		ImGui::Spacing();

		SDL_version version;
		SDL_GetVersion(&version);
		if (ImGui::Button("SDL")) { App->gui->RequestBrowser("https://www.libsdl.org/"); }
		ImGui::SameLine(); ImGui::TextWrapped("%i.%i.%i", version.major, version.minor, version.patch);

		if (ImGui::Button("OpenGL")) { App->gui->RequestBrowser("https://www.opengl.org/"); }
		ImGui::SameLine(); ImGui::TextWrapped("%s", glGetString(GL_VERSION));

		if (ImGui::Button("Glew")) { App->gui->RequestBrowser("http://glew.sourceforge.net/"); }
		ImGui::SameLine(); ImGui::TextWrapped("%s", glewGetString(GLEW_VERSION));

		if (ImGui::Button("ImGui")) { App->gui->RequestBrowser("https://github.com/ocornut/imgui"); }
		ImGui::SameLine(); ImGui::TextWrapped("%s", ImGui::GetVersion());

		if (ImGui::Button("ImGuizmo")) { App->gui->RequestBrowser("https://github.com/CedricGuillemet/ImGuizmo"); }
		ImGui::SameLine(); ImGui::TextWrapped("1.0");

		if (ImGui::Button("MathGeoLib")) { App->gui->RequestBrowser("https://github.com/juj/MathGeoLib"); }
		ImGui::SameLine(); ImGui::TextWrapped("1.5");

		if (ImGui::Button("JSON For Modern C++")) { App->gui->RequestBrowser("https://github.com/nlohmann/json"); }
		ImGui::SameLine(); ImGui::TextWrapped("3.7.0");

		if (ImGui::Button("DevIL")) { App->gui->RequestBrowser("http://openil.sourceforge.net/"); }
		ImGui::SameLine(); ImGui::TextWrapped("%i", IL_VERSION);

		if (ImGui::Button("Par shapes")) { App->gui->RequestBrowser("https://github.com/prideout/par/blob/master/par_shapes.h"); }

		if (ImGui::Button("MMGR")) { App->gui->RequestBrowser("http://www.paulnettle.com/"); }
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
