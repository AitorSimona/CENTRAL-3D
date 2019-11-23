#include "PanelProject.h"
#include "Application.h"
#include "ModuleFileSystem.h"



#include "Imgui/imgui.h"


#include "mmgr/mmgr.h"

PanelProject::PanelProject(char * name) : Panel(name)
{
}

PanelProject::~PanelProject()
{
}

bool PanelProject::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		RecursiveDirectoryDraw(ASSETS_FOLDER, nullptr);
	}

	ImGui::End();


	return true;
}

void PanelProject::RecursiveDirectoryDraw(const char * directory, const char * filter)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	App->fs->DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str()))
		{
			RecursiveDirectoryDraw((dir + (*it)).c_str(), filter);
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool ok = true;

		if (filter && str.substr(str.find_last_of(".") + 1) != filter)
			ok = false;

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Not using this at all due to data corruption
				ImGui::SetDragDropPayload("FBX", str.data(), str.size()); 

				dragged = ASSETS_FOLDER;
				dragged.append(str.data());
				ImGui::EndDragDropSource();
			}

			ImGui::TreePop();
		}
	}
}
