#include "PanelProject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"


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
		static std::vector<std::string> filters;
		RecursiveDirectoryDraw(ASSETS_FOLDER, filters);
	}

	ImGui::End();


	return true;
}

void PanelProject::RecursiveDirectoryDraw(const char * directory, std::vector<std::string>& filters)
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
			RecursiveDirectoryDraw((dir + (*it)).c_str(), filters);
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool pass_filter = false;

		if (filters.size() > 0)
		{
			for (uint i = 0; i < filters.size(); ++i)
			{
				if (str.substr(str.find_last_of(".") + 1) == filters[i])
				{
					pass_filter = true;
					break;
				}
			}
		}
		else
			pass_filter = true;

		if (pass_filter && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Not using this at all due to data corruption
				ImGui::SetDragDropPayload("FBX", str.data(), str.size()); 

				dragged = ASSETS_FOLDER;
				dragged.append(str.data());
				ImGui::EndDragDropSource();
			}

			std::string path = directory;
			path.append((*it).data());

			uint saved_date = App->resources->GetModDateFromMeta(path.data());

			// TO BE IMPLEMENTED 
			// --- If file is changed, reimport ---
			if (App->fs->GetLastModificationTime(path.data()) != saved_date)
			{

			}
			// --- If file is deleted, delete all related files in library and meta ---
			else if (saved_date == 0)
			{
				//uint uid = App->resources->GetUIDFromMeta(path.data());

				//std::string lib_path = MODELS_FOLDER;


			}

			ImGui::TreePop();
		}
	}
}
