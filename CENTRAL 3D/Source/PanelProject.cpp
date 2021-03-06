#include "PanelProject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleEventManager.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "Resources.h"
#include "Importers.h"
#include "GameObject.h"

#include "Imgui/imgui.h"


#include "mmgr/mmgr.h"
#include "ModuleInput.h"

// --- Event Manager Callbacks ---
void PanelProject::ONGameObjectSelected(const Event& e)
{
	App->gui->panelProject->SetSelected(nullptr);
}

void PanelProject::ONResourceDestroyed(const Event& e)
{
	if (e.uid == App->gui->panelProject->selected_uid)
		App->gui->panelProject->SetSelected(nullptr);
}

// -------------------------------

PanelProject::PanelProject(char* name) : Panel(name)
{
	// --- Add Event Listeners ---
	App->event_manager->AddListener(Event::EventType::GameObject_selected, ONGameObjectSelected);
	App->event_manager->AddListener(Event::EventType::Resource_destroyed, ONResourceDestroyed);

}

PanelProject::~PanelProject()
{
	App->event_manager->RemoveListener(Event::EventType::GameObject_selected, ONGameObjectSelected);
	App->event_manager->RemoveListener(Event::EventType::Resource_destroyed, ONResourceDestroyed);
}

// MYTODO: Clean this up !!!!!!!!!

bool PanelProject::Draw()
{
	ImGuiWindowFlags projectFlags = 0;
	projectFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	// --- Draw project panel, Unity style ---
	if (ImGui::Begin(name, &enabled, projectFlags))
	{
		// --- Create Resource Handling popup (now only resource creation) ---
		CreateResourceHandlingPopup();

		// --- Draw Menu Bar ---
		if (ImGui::BeginMenuBar())
			ImGui::EndMenuBar();

		// --- Draw Directories Tree ---
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 38));

		if (ImGui::BeginChild("AssetsTree", ImVec2(ImGui::GetWindowSize().x * 0.1, ImGui::GetWindowSize().y)))
			RecursiveDirectoryDraw(App->resources->GetAssetsFolder());

		ImGui::EndChild();

		// --- Draw Explorer ---
		ImGui::SameLine();

		projectFlags &= ~(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::BeginChild("AssetsExplorer", ImVec2(ImGui::GetWindowSize().x * 0.9f, ImGui::GetWindowSize().y), true, projectFlags))
		{			
			if (currentDirectory == nullptr)
				currentDirectory = App->resources->GetAssetsFolder();

			DrawFolder(currentDirectory);

			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));

			if (ImGui::InvisibleButton("##Drop Go", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() }))
				SetSelected(nullptr);

			// --- Drop a prefab instance, create another prefab ---
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GO"))
				{
					uint UID = *(const uint*)payload->Data;

					GameObject* go = App->scene_manager->currentScene->GetGOWithUID(UID);

					go->is_prefab_instance = true;

					CONSOLE_LOG("Created original prefab from: %s ...", go->GetName().c_str());

					// --- Build original new name ---
					std::string model_name;

					if (go->model)
					{
						model_name = go->model->GetName();
					}
					else
						model_name = go->GetName();

					model_name = model_name.substr(0, model_name.find("."));
					model_name.append(" (");
					std::string currdirectory = currentDirectory->GetResourceFile();
					std::string resource_name;
					uint instance = 0;

					resource_name = currdirectory + model_name + std::to_string(instance) + std::string(").prefab");

					while (App->fs->Exists(resource_name.c_str()))
					{
						instance++;
						resource_name = currdirectory + model_name + std::to_string(instance) + std::string(").prefab");
					}

					ResourcePrefab* new_prefab = (ResourcePrefab*)App->resources->CreateResource(Resource::ResourceType::PREFAB, resource_name);
	
					if (go->model)
						new_prefab->model = go->model;		

					new_prefab->parentgo = go;

					// --- Create new preview icon ---
					std::string previewTexpath;
					std::vector<GameObject*> prefab_gos;
					App->scene_manager->GatherGameObjects(new_prefab->parentgo, prefab_gos);
					uint texID = App->renderer3D->RenderSceneToTexture(prefab_gos, previewTexpath);
					new_prefab->previewTexPath = previewTexpath;
					new_prefab->SetPreviewTexID(texID);

					App->resources->AddResourceToFolder(new_prefab);

					// --- Create meta ---
					ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
					ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, resource_name, new_prefab->GetUID());

					if (meta)
						IMeta->Save(meta);

					ImporterPrefab* IPrefab = App->resources->GetImporter<ImporterPrefab>();
					IPrefab->Save((ResourcePrefab*)new_prefab);
					
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 58));

			// --- Item resizer and selected resource path display ---
			if (ImGui::BeginChild("ExplorerItemResizer", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
			{
				if (ImGui::BeginMenuBar())
				{
					if (selected)
						ImGui::Text(selected->GetName());

					ImGui::Spacing();

					ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.9f, ImGui::GetWindowPos().y));

					int imageSize_modifier = imageSize_px;
					ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * 0.1f);
					if (ImGui::SliderInt("##itemresizer", &imageSize_modifier, 32, 64))
					{
						imageSize_px = imageSize_modifier;
					}

					ImGui::EndMenuBar();
				}
			}

			ImGui::EndChild();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	ImGui::PopStyleVar();

	return true;
}

void PanelProject::CreateResourceHandlingPopup()
{
	// --- Delete resource ---
	if (selected && !selected->has_parent && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		delete_selected = true;

	if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_ChildWindows))
		ImGui::OpenPopup("Resources");

	if (ImGui::BeginPopup("Resources"))
	{

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
				createFolder = true;

			if (ImGui::MenuItem("Material"))
				createMaterial = true;

			if (ImGui::MenuItem("Shader"))
				createShader = true;

			if (ImGui::MenuItem("Scene"))
				createScene = true;

			ImGui::EndMenu();
		}

		if (selected && !selected->has_parent)
		{
			if (ImGui::MenuItem("Delete"))
				delete_selected = true;

			if (ImGui::MenuItem("Rename"))
			{
				rename_selected = true;
			}
		}
		ImGui::EndPopup();
	}

	if (createScene)
		ImGui::OpenPopup("Create new scene");

	if (ImGui::BeginPopupModal("Create new scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static char scene_name[50] = "NewScene";
		if (ImGui::InputText("NewScene", scene_name, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {

			Resource* new_scene = App->resources->CreateResource(Resource::ResourceType::SCENE, std::string(currentDirectory->GetResourceFile()).append(scene_name).append(".scene").c_str());
			ImporterScene* IScene = App->resources->GetImporter<ImporterScene>();

			App->resources->AddResourceToFolder(new_scene);

			// --- Create meta ---
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, new_scene->GetResourceFile(), new_scene->GetUID());

			if (meta)
				IMeta->Save(meta);

			IScene->SaveSceneToFile((ResourceScene*)new_scene);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Close", ImVec2(300, 0)))
			ImGui::CloseCurrentPopup();

		createScene = false;
		ImGui::EndPopup();
	}

	if (createMaterial)
		ImGui::OpenPopup("Create new material");

	if (ImGui::BeginPopupModal("Create new material", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static char mat_name[50] = "NewMaterial";
		if (ImGui::InputText("NewMaterial", mat_name, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {

			Resource* new_mat = App->resources->CreateResource(Resource::ResourceType::MATERIAL, std::string(currentDirectory->GetResourceFile()).append(mat_name).append(".mat").c_str());
			ImporterMaterial* IMat = App->resources->GetImporter<ImporterMaterial>();

			App->resources->AddResourceToFolder(new_mat);

			// --- Create meta ---
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, new_mat->GetResourceFile(), new_mat->GetUID());

			if (meta)
				IMeta->Save(meta);

			IMat->Save((ResourceMaterial*)new_mat);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Close", ImVec2(300, 0)))
			ImGui::CloseCurrentPopup();

		createMaterial = false;
		ImGui::EndPopup();
	}

	if (createFolder)
		ImGui::OpenPopup("Create new folder");

	if (ImGui::BeginPopupModal("Create new folder", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static char folder_name[50] = "NewFolder";
		if (ImGui::InputText("NewFolder", folder_name, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {

			Resource* new_folder = App->resources->CreateResource(Resource::ResourceType::FOLDER, std::string(currentDirectory->GetResourceFile()).append(folder_name).append("/").c_str());
			ImporterFolder* IFolder = App->resources->GetImporter<ImporterFolder>();

			//EngineApp->resources->AddResourceToFolder(new_folder);

			// --- Create meta ---
			std::string meta_path = new_folder->GetResourceFile();
			meta_path.pop_back();
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, meta_path, new_folder->GetUID());

			if (meta)
				IMeta->Save(meta);

			IFolder->Save((ResourceFolder*)new_folder);

			App->resources->AddResourceToFolder(new_folder);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Close", ImVec2(300, 0)))
			ImGui::CloseCurrentPopup();

		createFolder = false;
		ImGui::EndPopup();
	}

	if (createShader)
		ImGui::OpenPopup("Create new shader");

	if (ImGui::BeginPopupModal("Create new shader", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static char shader_name[50] = "NewShader";
		if (ImGui::InputText("NewShader", shader_name, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {

			Resource* new_shader = App->resources->CreateResource(Resource::ResourceType::SHADER, std::string(currentDirectory->GetResourceFile()).append(shader_name).append(".glsl").c_str());
			ImporterShader* IShader = App->resources->GetImporter<ImporterShader>();
			ResourceShader* shader = (ResourceShader*)new_shader;
			shader->ReloadAndCompileShader();

			//EngineApp->resources->AddResourceToFolder(new_shader);

			// --- Create meta ---
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, new_shader->GetResourceFile(), new_shader->GetUID());

			if (meta)
				IMeta->Save(meta);

			IShader->Save((ResourceShader*)new_shader);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Close", ImVec2(300, 0)))
			ImGui::CloseCurrentPopup();

		createShader = false;
		ImGui::EndPopup();
	}

	// --- Handle resource deletion ---
	if (delete_selected)
		ImGui::OpenPopup("Delete Selected Asset?");

	if (ImGui::BeginPopupModal("Delete Selected Asset?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("You are about to delete the selected asset. There is no going back!");

		if (ImGui::Button("Delete", ImVec2(300, 0)))
		{
			selected->OnDelete();

			if (selected->GetUID() != App->scene_manager->defaultScene->GetUID())
				delete selected;

			SetSelected(nullptr);
			delete_selected = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(300, 0)))
		{
			delete_selected = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// --- Handle resource deletion ---
	if (rename_selected)
		ImGui::OpenPopup("Rename Selected Asset?");

	if (ImGui::BeginPopupModal("Rename Selected Asset?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("You are about to rename the selected asset.");

		static char resName[100];
		strcpy_s(resName, 100,GetSelected()->GetName());

		if (ImGui::InputText("New name", resName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			GetSelected()->SetName(resName);
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(300, 0)))
		{
			rename_selected = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void PanelProject::SetSelected(Resource* new_selected)
{
	selected = new_selected;

	if (selected)
	{
		selected_uid = new_selected->GetUID();
		Event e(Event::EventType::Resource_selected);
		e.resource = selected;
		App->event_manager->PushEvent(e);
	}
	else
		selected_uid = 0;
}

const Resource* PanelProject::GetcurrentDirectory() const
{
	return currentDirectory;
}

Resource* PanelProject::GetSelected()
{
	return selected;
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
			ImGui::TextColored(ImVec4(0, 120, 255, 255), (*it)->GetName());
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
		maxColumns = ImGui::GetWindowSize().x / (imageSize_px + item_spacingX_px + 1);
		ImVec4 color = ImVec4(255, 255, 255, 255);

		ImVec2 vec = ImGui::GetCursorPos();

		// --- Draw sub-folders ---
		for (std::vector<ResourceFolder*>::const_iterator it = directories->begin(); it != directories->end(); ++it)
		{
			if (!*it)
				continue;

			ImGui::PushID((*it)->GetUID());

			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSize_px + item_spacingX_px) + item_spacingX_px);
			ImGui::SetCursorPosY(vec.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px);

			std::string item_name = (*it)->GetName();
			item_name.pop_back();
			LimitText(item_name);

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(0, 120, 255, 255);

			ImGui::Image((ImTextureID)(*it)->GetPreviewTexID(), ImVec2(imageSize_px, imageSize_px), ImVec2(0, 1), ImVec2(1, 0), color);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				uint UID = (*it)->GetUID();
				ImGui::SetDragDropPayload("resource", &UID, sizeof(uint));
				ImGui::Text(item_name.c_str());
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 20.0f));
				ImGui::Image((ImTextureID)(*it)->GetPreviewTexID(), ImVec2(imageSize_px, imageSize_px), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleVar();

			if (ImGui::IsItemClicked())
				SetSelected(*it);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				currentDirectory = *it;

			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSize_px + item_spacingX_px) + item_spacingX_px + ((imageSize_px - ImGui::CalcTextSize(item_name.c_str(), nullptr).x) / 2));
			ImGui::SetCursorPosY(vec.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px + imageSize_px);

			ImGui::TextColored(color, item_name.c_str());

			if (selected && selected->GetUID() == (*it)->GetUID())
				color = ImVec4(255, 255, 255, 255);

			ImGui::PopID();

			if (maxColumns != 0 && (i + 1) % maxColumns == 0)
				row++;

			i++;
		}

		// --- Draw the rest of files ---
		for (std::vector<Resource*>::const_iterator it = resources->begin(); it != resources->end(); ++it)
		{
			if (!*it)
				continue;

			if ((*it)->has_parent)
				continue;

			DrawFile(*it, i, row, vec, color);

			// --- Draw model childs ---
			if ((*it)->GetType() == Resource::ResourceType::MODEL)
			{
				uint arrowSize = imageSize_px / 4;

				ImGui::SetCursorPosX(vec.x + ((i + 1) - row * maxColumns) * (imageSize_px + item_spacingX_px));
				ImGui::SetCursorPosY(vec.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px + arrowSize * 1.5);

				ResourceModel* model = (ResourceModel*)*it;

				ImVec2 uvx = { 0,1 };
				ImVec2 uvy = { 1,0 };
				// --- Set arrow direction ---
				if (model->openInProject)
				{
					uvx = { 1,0 };
					uvy = { 0,1 };
				}

				// --- Force new uid so imgui does not block all buttons after the first one ---
				ImGui::PushID((*it)->GetUID() + i);

				if (ImGui::ImageButton((ImTextureID)App->gui->playbuttonTexID, ImVec2(arrowSize, arrowSize), uvx, uvy, 0))
					model->openInProject = !model->openInProject;

				ImGui::PopID();

				if (model->openInProject)
				{
					std::vector<Resource*>* model_resources = model->GetResources();

					for (std::vector<Resource*>::const_iterator res = model_resources->begin(); res != model_resources->end(); ++res)
					{
						if (maxColumns != 0 && (i + 1) % maxColumns == 0)
							row++;

						i++;

						DrawFile(*res, i, row, vec, color, true);
					}
				}

			}

			if (maxColumns != 0 && (i + 1) % maxColumns == 0)
				row++;

			i++;
		}
	}

	ImGui::PopStyleVar();
}

void PanelProject::DrawFile(Resource* resource, uint i, uint row, ImVec2& cursor_pos, ImVec4& color, bool child)
{
	ImGui::PushID(resource->GetUID());

	ImGui::SetCursorPosX(cursor_pos.x + (i - row * maxColumns) * (imageSize_px + item_spacingX_px) + item_spacingX_px);
	ImGui::SetCursorPosY(cursor_pos.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px);

	std::string item_name = resource->GetName();
	LimitText(item_name);

	if (selected && selected->GetUID() == resource->GetUID())
		color = ImVec4(0, 120, 255, 255);

	if (child)
		ImGui::ImageButton((ImTextureID)resource->GetPreviewTexID(), ImVec2(imageSize_px, imageSize_px), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0.15, 0.15, 0.15, 1), color);
	else
		ImGui::Image((ImTextureID)resource->GetPreviewTexID(), ImVec2(imageSize_px, imageSize_px), ImVec2(0, 1), ImVec2(1, 0), color);


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

	// --- Handle drag and drop origin ---
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		uint UID = resource->GetUID();
		ImGui::SetDragDropPayload("resource", &UID, sizeof(uint));
		ImGui::Text(item_name.c_str());
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 20.0f));
		ImGui::Image((ImTextureID)resource->GetPreviewTexID(), ImVec2(imageSize_px, imageSize_px), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndDragDropSource();
	}

	// --- Handle selection ---
	if (selected_uid == resource->GetUID() && wasclicked && ImGui::IsMouseReleased(0))
	{
		if (ImGui::IsItemHovered())
		{
			SetSelected(resource);
			wasclicked = false;
		}
		else
			SetSelected(nullptr);
	}

	if (ImGui::IsItemClicked())
	{
		selected_uid = resource->GetUID();
		wasclicked = true;
	}
	// --- Handle resource double click ---
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		// --- Load scene on double click ---
		if (resource->GetType() == Resource::ResourceType::SCENE)
			App->scene_manager->SetActiveScene((ResourceScene*)resource);

		// --- Open the rest of resources with OS defined default program ---
		else 
		{
			// MYTODO: Use win32 API to get the absolute path

			// --- Build absolute path for ShellExecute function ---
			std::string abs_path = App->fs->GetWorkingDirectory();

			abs_path += resource->GetOriginalFile();
			App->fs->NormalizePath(abs_path);
			App->gui->RequestBrowser(abs_path.c_str());
		}
	}

	ImGui::PopStyleVar();


	ImGui::SetCursorPosX(cursor_pos.x + (i - row * maxColumns) * (imageSize_px + item_spacingX_px) + item_spacingX_px + ((imageSize_px - ImGui::CalcTextSize(item_name.c_str(), nullptr).x) / 2));
	ImGui::SetCursorPosY(cursor_pos.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px + imageSize_px);

	ImGui::TextColored(color, item_name.c_str());

	if (selected && selected->GetUID() == resource->GetUID())
		color = ImVec4(255, 255, 255, 255);

	ImGui::PopID();
}

void PanelProject::LimitText(std::string& text)
{
	uint textSizeX_px = ImGui::CalcTextSize(text.c_str(), nullptr).x;
	uint dotsSizeX_px = ImGui::CalcTextSize("...", nullptr, false, 0).x;

	// --- The total pixel space available is the text picel space + dots pixel space ---
	if (imageSize_px < textSizeX_px)
	{
		uint charSizeX_px = textSizeX_px / text.size();
		text = text.substr(0, (imageSize_px - dotsSizeX_px) / charSizeX_px);
		text.append("...");
	}
}

void PanelProject::RecursiveDirectoryDraw(ResourceFolder* folder)
{
	std::vector<ResourceFolder*> childs = folder->GetChilds();

	for (std::vector<ResourceFolder*>::iterator it = childs.begin(); it != childs.end(); ++it) 
	{
		std::string dir_name = folder->GetName();
		std::string child_name = (*it)->GetName();

		if (ImGui::TreeNodeEx((dir_name + child_name).c_str(), 0, "%s", child_name.c_str())) 
		{
			RecursiveDirectoryDraw(*it);
			ImGui::TreePop();
		}
	}
}
