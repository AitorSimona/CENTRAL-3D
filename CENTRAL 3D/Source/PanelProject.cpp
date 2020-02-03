#include "PanelProject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleEventManager.h"
#include "ModuleGui.h"

#include "ResourceFolder.h"

#include "Imgui/imgui.h"


#include "mmgr/mmgr.h"

// --- Event Manager Callbacks ---
void PanelProject::ONGameObjectSelected(const Event& e)
{
	App->gui->panelProject->SetSelected(nullptr);
}

// -------------------------------

PanelProject::PanelProject(char * name) : Panel(name)
{
	// --- Add Event Listeners ---
	App->event_manager->AddListener(Event::EventType::GameObject_selected, ONGameObjectSelected);
}

PanelProject::~PanelProject()
{
	App->event_manager->AddListener(Event::EventType::GameObject_selected, ONGameObjectSelected);
}


// MYTODO: Clean this up

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

		ImGui::BeginChild("AssetsExplorer", ImVec2(ImGui::GetWindowSize().x*0.9f, ImGui::GetWindowSize().y*0.9f), true, projectFlags);

		if(currentDirectory == nullptr)
			currentDirectory = App->resources->GetAssetsFolder();

		DrawFolder(currentDirectory);

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

void PanelProject::SetSelected(Resource* new_selected)
{
	selected = new_selected;

	if (selected)
	{
		Event e(Event::EventType::Resource_selected);
		e.resource = selected;
		App->event_manager->PushEvent(e);
	}
}


void PanelProject::DrawFolder(ResourceFolder* folder)
{
	// --- Draw menuBar / path to current folder ---
	ImGui::BeginMenuBar();

	ResourceFolder* curr = folder;

	if (currentDirectory == App->resources->GetAssetsFolder())
		ImGui::TextColored(ImVec4(0, 120, 255, 255), App->resources->GetAssetsFolder()->GetName());
	else
	ImGui::Text(App->resources->GetAssetsFolder()->GetName());

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		currentDirectory = App->resources->GetAssetsFolder();

	std::vector<ResourceFolder*> folders_path;

	while (curr->GetParent())
	{
		folders_path.push_back(curr);
		curr = curr->GetParent();
	}

	for (std::vector<ResourceFolder*>::const_reverse_iterator it = folders_path.rbegin(); it != folders_path.rend(); ++it)
	{
		if (currentDirectory == *it)
			ImGui::TextColored(ImVec4(0, 120, 255, 255),(*it)->GetName());
		else
		ImGui::Text((*it)->GetName());

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			currentDirectory = *it;

		ImGui::SameLine();
	}

	ImGui::EndMenuBar();


	// --- Draw File Explorer ---

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacingX_px, item_spacingY_px));

	if (folder)
	{
		const std::vector<Resource*>* resources = &folder->GetResources();
		const std::vector<ResourceFolder*>* directories = &folder->GetChilds();
		uint i = 0;
		uint row = 0;
		maxColumns = ImGui::GetWindowSize().x / (imageSizeX_px + item_spacingX_px);
		ImVec4 color = ImVec4(255, 255, 255, 255);

		ImVec2 vec = ImGui::GetCursorPos();

		// --- Draw sub-folders ---
		for (std::vector<ResourceFolder*>::const_iterator it = directories->begin(); it != directories->end(); ++it)
		{
			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSizeX_px + item_spacingX_px) + item_spacingX_px);
			ImGui::SetCursorPosY(vec.y + row * (imageSizeY_px + item_spacingY_px) + item_spacingY_px);

			std::string item_name = (*it)->GetName();
			item_name.pop_back();
			LimitText(item_name);

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(0, 120, 255, 255);

			ImGui::Image((ImTextureID)(*it)->GetPreviewTexUID(), ImVec2(imageSizeX_px, imageSizeY_px), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::IsItemClicked())
				SetSelected(*it);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				currentDirectory = *it;

			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSizeX_px + item_spacingX_px) + item_spacingX_px + ((imageSizeX_px - ImGui::CalcTextSize(item_name.c_str(), nullptr).x)/2));
			ImGui::SetCursorPosY(vec.y + row * (imageSizeY_px + item_spacingY_px) + item_spacingY_px + imageSizeY_px);

			ImGui::TextColored(color, item_name.c_str());

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(255, 255, 255, 255);

			if ((i + 1) % maxColumns == 0)
				row++;
			else
				ImGui::SameLine();

			i++;
		}

		// --- Draw the rest of files ---
		for (std::vector<Resource*>::const_iterator it = resources->begin(); it != resources->end(); ++it)
		{
			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns)* (imageSizeX_px + item_spacingX_px) + item_spacingX_px);
			ImGui::SetCursorPosY(vec.y + row * (imageSizeY_px + item_spacingY_px) + item_spacingY_px);

			std::string item_name = (*it)->GetName();
			LimitText(item_name);

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(0, 120, 255, 255);

			ImGui::Image((ImTextureID)(*it)->GetPreviewTexUID(), ImVec2(imageSizeX_px, imageSizeY_px), ImVec2(0, 1), ImVec2(1, 0),color);

			if (ImGui::IsItemClicked())
				SetSelected(*it);


			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSizeX_px + item_spacingX_px) + item_spacingX_px + ((imageSizeX_px - ImGui::CalcTextSize(item_name.c_str(), nullptr).x) / 2));
			ImGui::SetCursorPosY(vec.y + row * (imageSizeY_px + item_spacingY_px) + item_spacingY_px + imageSizeY_px);

			ImGui::TextColored(color,item_name.c_str());

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(255, 255, 255, 255);

			if ((i + 1) % maxColumns == 0)
				row++;
			else
				ImGui::SameLine();

			i++;
		}
	}

	ImGui::PopStyleVar();
}

void PanelProject::LimitText(std::string& text)
{
	uint textSizeX_px = ImGui::CalcTextSize(text.c_str(),nullptr).x;
	uint dotsSizeX_px = ImGui::CalcTextSize("...", nullptr, false, 0).x;

	// --- The total pixel space available is the text picel space + dots pixel space ---
	if (imageSizeX_px < textSizeX_px)
	{
		uint charSizeX_px = textSizeX_px / text.size();
		text = text.substr(0, (imageSizeX_px - dotsSizeX_px) / charSizeX_px);
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

	//std::sort(files.begin(), files.end());

	//for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	//{
	//	const std::string& str = *it;

	//	bool pass_filter = false;

	//	if (filters.size() > 0)
	//	{
	//		for (uint i = 0; i < filters.size(); ++i)
	//		{
	//			if (str.substr(str.find_last_of(".") + 1) == filters[i])
	//			{
	//				pass_filter = true;
	//				break;
	//			}
	//		}
	//	}
	//	else
	//		pass_filter = true;

	//	//if (pass_filter && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
	//	//{
	//	//	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	//	//	{
	//	//		// Not using this at all due to data corruption
	//	//		ImGui::SetDragDropPayload("FBX", str.data(), str.size()); 

	//	//		dragged = ASSETS_FOLDER;
	//	//		dragged.append(str.data());
	//	//		ImGui::EndDragDropSource();
	//	//	}

	//	//	std::string path = directory;
	//	//	path.append((*it).data());

	//	//	//uint saved_date = App->resources->GetModDateFromMeta(path.data());

	//	//	//// TO BE IMPLEMENTED 
	//	//	//// --- If file is changed, reimport ---
	//	//	//if (App->fs->GetLastModificationTime(path.data()) != saved_date)
	//	//	//{

	//	//	//}
	//	//	//// --- If file is deleted, delete all related files in library and meta ---
	//	//	//else if (saved_date == 0)
	//	//	//{
	//	//	//	//uint uid = App->resources->GetUIDFromMeta(path.data());

	//	//	//	//std::string lib_path = MODELS_FOLDER;


	//	//	//}

	//	//	ImGui::TreePop();
	//	//}
	//}
}
