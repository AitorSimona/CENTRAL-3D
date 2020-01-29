#include "PanelProject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "ResourceFolder.h"

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
	ImGuiWindowFlags projectFlags = 0;
	projectFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	// --- Draw project panel, Unity style ---
	if (ImGui::Begin(name, &enabled, projectFlags))
	{
		static std::vector<std::string> filters;

		ImGui::BeginMenuBar();
		ImGui::EndMenuBar();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 38));

		// --- Draw Directories Tree ---
		ImGui::BeginChild("AssetsTree", ImVec2(ImGui::GetWindowSize().x*0.10f,ImGui::GetWindowSize().y));

		RecursiveDirectoryDraw(ASSETS_FOLDER, filters);

		ImGui::EndChild();

		// --- Draw Explorer ---
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

		ImGui::BeginChild("AssetsExplorer", ImVec2(ImGui::GetWindowSize().x*0.9f, ImGui::GetWindowSize().y*0.9f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);

		DrawFolder(App->resources->GetAssetsFolder());

		ImGui::PopStyleVar();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 20));

		// --- Item resizer To be Implemented!! ---
		ImGui::BeginChild("ExplorerItemResizer", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y * 0.1f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);
		ImGui::BeginMenuBar();
		ImGui::EndMenuBar();
		ImGui::EndChild();

		ImGui::EndChild();
	}

	ImGui::PopStyleVar();

	ImGui::End();


	return true;
}

void PanelProject::DrawFolder(ResourceFolder* folder)
{
	//ImGui::BeginChild("Directory",ImVec2(0,0),true, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();

	ImGui::Text(folder->GetName());

	ImGui::EndMenuBar();

	//ImGui::BeginChild("Explorer");

	if (folder)
	{
		const std::vector<Resource*>* resources = &folder->GetResources();

		for (std::vector<Resource*>::const_iterator it = resources->begin(); it != resources->end(); ++it)
		{
			std::string item_name = (*it)->GetName();

			LimitText(item_name);

			ImGui::Text(item_name.c_str());
		}
	}

	//ImGui::EndChild();

	//ImGui::EndChild();
}

void PanelProject::LimitText(std::string& text)
{
	uint max_size = 10;
	uint text_pxsize = ImGui::CalcTextSize(text.c_str(), "").x;

	if (max_size < text_pxsize)
	{
		text = text.substr(0, max_size - 3);
		text.append("...");
	}
}

// MYTODO: To be substituted (folders/files are already loaded)
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

			//uint saved_date = App->resources->GetModDateFromMeta(path.data());

			//// TO BE IMPLEMENTED 
			//// --- If file is changed, reimport ---
			//if (App->fs->GetLastModificationTime(path.data()) != saved_date)
			//{

			//}
			//// --- If file is deleted, delete all related files in library and meta ---
			//else if (saved_date == 0)
			//{
			//	//uint uid = App->resources->GetUIDFromMeta(path.data());

			//	//std::string lib_path = MODELS_FOLDER;


			//}

			ImGui::TreePop();
		}
	}
}
