#include "PanelProject.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "ModuleEventManager.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleSceneManager.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"

// -- Resources --
#include "ResourceScene.h"
#include "ResourceModel.h"
#include "ResourceFolder.h"
#include "ResourcePrefab.h"
#include "ResourceShader.h"

// -- Importers --
#include "ImporterMeta.h"
#include "ImporterPrefab.h"
#include "ImporterFolder.h"
#include "ImporterMaterial.h"
#include "ImporterShader.h"
#include "ImporterScene.h"
#include "ImporterScript.h"

// -- Components --
#include "GameObject.h"

// -- Utilities --
#include "Imgui/imgui.h"
#include <memory>
#include <iostream>
#include <fstream>
#include "mmgr/mmgr.h"

// --- Event Manager Callbacks ---
void PanelProject::ONGameObjectSelected(const Broken::Event& e)
{
	EngineApp->editorui->panelProject->SetSelected(nullptr);
}

void PanelProject::ONResourceDestroyed(const Broken::Event& e)
{
	if (e.uid == EngineApp->editorui->panelProject->selected_uid)
		EngineApp->editorui->panelProject->SetSelected(nullptr);
}

// -------------------------------

PanelProject::PanelProject(char * name) : Panel(name)
{
	// --- Add Event Listeners ---
	EngineApp->event_manager->AddListener(Broken::Event::EventType::GameObject_selected, ONGameObjectSelected);
	EngineApp->event_manager->AddListener(Broken::Event::EventType::Resource_destroyed, ONResourceDestroyed);

}

PanelProject::~PanelProject()
{
	EngineApp->event_manager->RemoveListener(Broken::Event::EventType::GameObject_selected, ONGameObjectSelected);
	EngineApp->event_manager->RemoveListener(Broken::Event::EventType::Resource_destroyed, ONResourceDestroyed);
}


// MYTODO: Clean this up !!!!!!!!!

bool PanelProject::Draw()
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags projectFlags = 0;
	projectFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	// --- Draw project panel, Unity style ---
	if (ImGui::Begin(name, &enabled, projectFlags))
	{
		CreateResourceHandlingPopup();

		if (ImGui::BeginMenuBar())
			ImGui::EndMenuBar();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 38));

		// --- Draw Directories Tree ---
		if (ImGui::BeginChild("AssetsTree", ImVec2(ImGui::GetWindowSize().x * 0.1, ImGui::GetWindowSize().y)))
			RecursiveDirectoryDraw(EngineApp->resources->GetAssetsFolder());
		ImGui::EndChild();

		// --- Draw Explorer ---
		ImGui::SameLine();

		if (ImGui::BeginChild("AssetsExplorer", ImVec2(ImGui::GetWindowSize().x * 0.9f, ImGui::GetWindowSize().y), true, projectFlags)) {

			if (currentDirectory == nullptr)
				currentDirectory = EngineApp->resources->GetAssetsFolder();

			DrawFolder(EngineApp->resources->getCurrentDirectory());

			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));

			ImGui::InvisibleButton("##Drop Go", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });

			// --- Drop a prefab instance, create another prefab ---
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GO"))
				{
					uint UID = *(const uint*)payload->Data;

					Broken::GameObject* go = EngineApp->scene_manager->currentScene->GetGOWithUID(UID);

					// --- Block move if go is prefab child ---
					go->is_prefab_instance = true;
					
					//ENGINE_CONSOLE_LOG("Created original prefab from: %s ...", go->GetName().c_str());

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

					while (EngineApp->fs->Exists(resource_name.c_str()))
					{
						instance++;
						resource_name = currdirectory + model_name + std::to_string(instance) + std::string(").prefab");
					}

					Broken::ResourcePrefab* new_prefab = (Broken::ResourcePrefab*)EngineApp->resources->CreateResource(Broken::Resource::ResourceType::PREFAB, resource_name.c_str());

					if(go->model)
						new_prefab->model = go->model;

					new_prefab->parentgo = go;

					// --- Create new preview icon ---
					std::vector <Broken::GameObject*> prefab_gos;
					GatherGameObjects(new_prefab->parentgo, prefab_gos);
					uint texID = 0;
					new_prefab->previewTexPath = EngineApp->renderer3D->RenderSceneToTexture(prefab_gos, texID);
					new_prefab->SetPreviewTexID(texID);

					EngineApp->resources->AddResourceToFolder(new_prefab);

					// --- Create meta ---
					Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
					Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, resource_name.c_str(), new_prefab->GetUID());

					if (meta)
						IMeta->Save(meta);

					Broken::ImporterPrefab* IPrefab = EngineApp->resources->GetImporter<Broken::ImporterPrefab>();
					IPrefab->Save((Broken::ResourcePrefab*)new_prefab);
					
				}
				ImGui::EndDragDropTarget();
			}
			
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 58));


			// --- Item resizer and selected resource path display ---
			if (ImGui::BeginChild("ExplorerItemResizer", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
			{
				if (ImGui::BeginMenuBar()) {

					if (selected)
						ImGui::Text(selected->GetName());

					ImGui::Spacing();

					ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.9f, ImGui::GetWindowPos().y));

					int imageSize_modifier = imageSize_px;
					ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * 0.1f);
					if (ImGui::SliderInt("##itemresizer", &imageSize_modifier, 32, 64)) {
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
	if (selected && !selected->has_parent && EngineApp->input->GetKey(SDL_SCANCODE_DELETE) == Broken::KEY_DOWN)
		delete_selected = true;
	//ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());
	// Call the more complete ShowExampleMenuFile which we use in various places of this demo
	if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_ChildWindows))
		ImGui::OpenPopup("Resources");

	if (ImGui::BeginPopup("Resources"))
	{
		//ImGui::MenuItem("(dummy menu)", NULL, false, false);
		//if (ImGui::MenuItem("New")) {}
		//if (ImGui::MenuItem("Open", "Ctrl+O")) {}


		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				std::string resource_name;
				resource_name = *(EngineApp->resources->GetNewUniqueName(Broken::Resource::ResourceType::FOLDER));

				Broken::Resource* new_folder = EngineApp->resources->CreateResource(Broken::Resource::ResourceType::FOLDER, std::string(currentDirectory->GetResourceFile()).append(resource_name).c_str());
				Broken::ImporterFolder* IFolder = EngineApp->resources->GetImporter<Broken::ImporterFolder>();

				EngineApp->resources->AddResourceToFolder(new_folder);

				resource_name.pop_back();
				// --- Create meta ---
				Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
				Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, std::string(currentDirectory->GetResourceFile()).append(resource_name).c_str(), new_folder->GetUID());

				if (meta)
					IMeta->Save(meta);

				IFolder->Save((Broken::ResourceFolder*)new_folder);
			}

			if (ImGui::MenuItem("Material"))
			{
				std::string resource_name;
				resource_name = *(EngineApp->resources->GetNewUniqueName(Broken::Resource::ResourceType::MATERIAL));

				Broken::Resource* new_material = EngineApp->resources->CreateResource(Broken::Resource::ResourceType::MATERIAL, std::string(currentDirectory->GetResourceFile()).append(resource_name).c_str());
				Broken::ImporterMaterial* IMat = EngineApp->resources->GetImporter<Broken::ImporterMaterial>();

				EngineApp->resources->AddResourceToFolder(new_material);

				// --- Create meta ---
				Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
				Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, new_material->GetResourceFile(), new_material->GetUID());

				if (meta)
					IMeta->Save(meta);

				IMat->Save((Broken::ResourceMaterial*)new_material);
			}

			if (ImGui::MenuItem("Shader"))
			{
				std::string resource_name;
				resource_name = *(EngineApp->resources->GetNewUniqueName(Broken::Resource::ResourceType::SHADER));

				Broken::Resource* new_shader = EngineApp->resources->CreateResource(Broken::Resource::ResourceType::SHADER, std::string(currentDirectory->GetResourceFile()).append(resource_name).c_str());
				Broken::ImporterShader* IShader = EngineApp->resources->GetImporter<Broken::ImporterShader>();
				Broken::ResourceShader* shader = (Broken::ResourceShader*)new_shader;
				shader->ReloadAndCompileShader();

				EngineApp->resources->AddResourceToFolder(new_shader);

				// --- Create meta ---
				Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
				Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, new_shader->GetOriginalFile(), new_shader->GetUID());

				if (meta)
					IMeta->Save(meta);

				IShader->Save((Broken::ResourceShader*)new_shader);
			}

			if (ImGui::MenuItem("Script"))
			{
				createScript = true;
			}

			if (ImGui::MenuItem("Scene"))
			{
				std::string resource_name;
				resource_name = *(EngineApp->resources->GetNewUniqueName(Broken::Resource::ResourceType::SCENE));

				Broken::Resource* new_scene = EngineApp->resources->CreateResource(Broken::Resource::ResourceType::SCENE, std::string(currentDirectory->GetResourceFile()).append(resource_name).c_str());
				Broken::ImporterScene* IScene = EngineApp->resources->GetImporter<Broken::ImporterScene>();

				EngineApp->resources->AddResourceToFolder(new_scene);

				// --- Create meta ---
				Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
				Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, new_scene->GetResourceFile(), new_scene->GetUID());

				if (meta)
					IMeta->Save(meta);

				IScene->SaveSceneToFile((Broken::ResourceScene*)new_scene);
			}

			ImGui::EndMenu();
		}
		if (selected && !selected->has_parent)
		{
			if (ImGui::MenuItem("Delete"))
				delete_selected = true;		
		}
		ImGui::EndPopup();
	}

	if (delete_selected)
		ImGui::OpenPopup("Delete Selected Asset?");

	if (ImGui::BeginPopupModal("Delete Selected Asset?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("You are about to delete the selected asset. There is no going back!");

		if (ImGui::Button("Delete", ImVec2(300, 0)))
		{
			EngineApp->resources->ForceDelete(GetSelected());
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

	if (createScript)
		ImGui::OpenPopup("Create new script");

	if (ImGui::BeginPopupModal("Create new script", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static char script_name[50] = "NewScript";
		if (ImGui::InputText("Script name", script_name, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
			
			Broken::Resource* new_script = EngineApp->resources->CreateResource(Broken::Resource::ResourceType::SCRIPT, std::string(currentDirectory->GetResourceFile()).append(script_name).append(".lua").c_str());
			Broken::ImporterScript* IScript = EngineApp->resources->GetImporter<Broken::ImporterScript>();

			EngineApp->resources->AddResourceToFolder(new_script);

			// --- Create meta ---
			Broken::ImporterMeta* IMeta = EngineApp->resources->GetImporter<Broken::ImporterMeta>();
			Broken::ResourceMeta* meta = (Broken::ResourceMeta*)EngineApp->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, new_script->GetResourceFile(), new_script->GetUID());

			if (meta)
				IMeta->Save(meta);

			IScript->Save((Broken::ResourceScript*)new_script);

			// Initializate script
			std::string full_path = new_script->GetResourceFile();
			std::string s_name = script_name;

			std::ofstream file;
			file.open(full_path);
			file << "function GetTable" + s_name + "()\n";
			file << "local lua_table = {}\n";
			file << "lua_table.System = Scripting.System()\n\n";

			file << "function lua_table:Awake()\n";
			file << "end\n\n";

			file << "function lua_table:Start()\n";
			file << "end\n\n";

			file << "function lua_table:Update()\n";
			file << "end\n\n";

			file << "return lua_table\n";
			file << "end";
			file.close();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Close", ImVec2(300, 0)))
			ImGui::CloseCurrentPopup();

		createScript = false;
		ImGui::EndPopup();
	}
}

void PanelProject::SetSelected(Broken::Resource* new_selected)
{
	selected = new_selected;

	if (selected)
	{
		selected_uid = new_selected->GetUID();
		Broken::Event e(Broken::Event::EventType::Resource_selected);
		e.resource = selected;
		EngineApp->event_manager->PushEvent(e);
	}
	else
		selected_uid = 0;
}

Broken::Resource* PanelProject::GetSelected()
{
	return selected;
}

//const Broken::Resource* PanelProject::GetcurrentDirectory() const
//{
//	return currentDirectory;
//}


void PanelProject::DrawFolder(Broken::ResourceFolder* folder)
{
	// --- Draw menuBar / path to current folder ---
	ImGui::BeginMenuBar();

	Broken::ResourceFolder* curr = folder;

	if (EngineApp->resources->getCurrentDirectory() == EngineApp->resources->GetAssetsFolder())
		ImGui::TextColored(ImVec4(0, 120, 255, 255), EngineApp->resources->GetAssetsFolder()->GetName());
	else
	ImGui::Text(EngineApp->resources->GetAssetsFolder()->GetName());

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
		currentDirectory = EngineApp->resources->GetAssetsFolder();
		EngineApp->resources->setCurrentDirectory(EngineApp->resources->GetAssetsFolder());
	}

	std::vector<Broken::ResourceFolder*> folders_path;

	while (curr->GetParent())
	{
		folders_path.push_back(curr);
		curr = curr->GetParent();
	}

	for (std::vector<Broken::ResourceFolder*>::const_reverse_iterator it = folders_path.rbegin(); it != folders_path.rend(); ++it)
	{
		if (EngineApp->resources->getCurrentDirectory() == *it)
			ImGui::TextColored(ImVec4(0, 120, 255, 255),(*it)->GetName());
		else
			ImGui::Text((*it)->GetName());

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			currentDirectory = *it;
			EngineApp->resources->setCurrentDirectory(*it);
		}

		ImGui::SameLine();
	}

	ImGui::EndMenuBar();


	// --- Draw File Explorer ---

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacingX_px, item_spacingY_px));

	if (folder)
	{
		const std::vector<Broken::Resource*>* resources = &folder->GetResources();
		const std::vector<Broken::ResourceFolder*>* directories = &folder->GetChilds();
		uint i = 0;
		uint row = 0;
		maxColumns = ImGui::GetWindowSize().x / (imageSize_px + item_spacingX_px + 1);
		ImVec4 color = ImVec4(255, 255, 255, 255);

		ImVec2 vec = ImGui::GetCursorPos();

		// --- Draw sub-folders ---
		for (std::vector<Broken::ResourceFolder*>::const_iterator it = directories->begin(); it != directories->end(); ++it)
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

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				currentDirectory = *it;
				EngineApp->resources->setCurrentDirectory(*it);
			}

			ImGui::SetCursorPosX(vec.x + (i - row * maxColumns) * (imageSize_px + item_spacingX_px) + item_spacingX_px + ((imageSize_px - ImGui::CalcTextSize(item_name.c_str(), nullptr).x)/2));
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
		for (std::vector<Broken::Resource*>::const_iterator it = resources->begin(); it != resources->end(); ++it)
		{
			if (!*it)
				continue;

			if ((*it)->has_parent)
				continue;

			DrawFile(*it, i, row, vec, color);

			// --- Draw model childs ---
			if ((*it)->GetType() == Broken::Resource::ResourceType::MODEL)
			{
				uint arrowSize = imageSize_px / 4;

				ImGui::SetCursorPosX(vec.x + ((i + 1) - row * maxColumns) * (imageSize_px + item_spacingX_px));
				ImGui::SetCursorPosY(vec.y + row * (imageSize_px + item_spacingY_px) + item_spacingY_px + arrowSize*1.5);

				Broken::ResourceModel* model = (Broken::ResourceModel*)*it;

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

				if (ImGui::ImageButton((ImTextureID)EngineApp->gui->playbuttonTexID, ImVec2(arrowSize, arrowSize), uvx, uvy, 0))
					model->openInProject = !model->openInProject;

				ImGui::PopID();

				if (model->openInProject)
				{
					std::vector<Broken::Resource*>* model_resources = model->GetResources();

					for (std::vector<Broken::Resource*>::const_iterator res = model_resources->begin(); res != model_resources->end(); ++res)
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

void PanelProject::DrawFile(Broken::Resource* resource, uint i, uint row, ImVec2& cursor_pos, ImVec4& color, bool child)
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

	// --- Handle selection ---
	if (ImGui::IsItemClicked())
	{
		selected_uid = resource->GetUID();
		//selected = resource;
		wasclicked = true;
	}

	// --- Handle resource double click ---
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		// If it is a scene, load it
		if (resource->GetType() == Broken::Resource::ResourceType::SCENE)
			EngineApp->scene_manager->SetActiveScene((Broken::ResourceScene*)resource);
		// Open resources and files with default program
		else {
			// Construct absolute path for ShellExecute function
			std::string abs_path = EngineApp->fs->GetBasePath();

			std::size_t d_pos = 0;
			d_pos = abs_path.find("Debug");
			std::size_t r_pos = 0;
			r_pos = abs_path.find("Release");

			if (d_pos != 4294967295)  // If we are in DEBUG
			{
				abs_path = abs_path.substr(0, d_pos);
				abs_path += "Game/";
			}
			else if (r_pos != 4294967295) // If we are in RELEASE
			{
				abs_path = abs_path.substr(0, r_pos);
				abs_path += "Game/";
			}

			abs_path += resource->GetOriginalFile();
			EngineApp->fs->NormalizePath(abs_path);
			EngineApp->gui->RequestBrowser(abs_path.c_str());
		}
	}

	//if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete), false))
	//	EngineApp->resources->ONResourceDestroyed

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
	uint textSizeX_px = ImGui::CalcTextSize(text.c_str(),nullptr).x;
	uint dotsSizeX_px = ImGui::CalcTextSize("...", nullptr, false, 0).x;

	// --- The total pixel space available is the text picel space + dots pixel space ---
	if (imageSize_px < textSizeX_px)
	{
		uint charSizeX_px = textSizeX_px / text.size();
		text = text.substr(0, (imageSize_px - dotsSizeX_px) / charSizeX_px);
		text.append("...");
	}
}

// MYTODO: To be substituted (folders/files are already loaded)
void PanelProject::RecursiveDirectoryDraw(Broken::ResourceFolder* directory)
{
	std::vector<Broken::ResourceFolder*> childs = directory->GetChilds();
	for (std::vector<Broken::ResourceFolder*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		std::string dir_name = directory->GetName();
		std::string child_name = (*it)->GetName();
		if (ImGui::TreeNodeEx((dir_name + child_name).c_str(), 0, "%s/", child_name.c_str())) {
			RecursiveDirectoryDraw(*it);
			ImGui::TreePop();
		}
	}
}

void PanelProject::GatherGameObjects(Broken::GameObject* go, std::vector<Broken::GameObject*>& gos_vec)
{
	gos_vec.push_back(go);

	for (uint i = 0; i < go->childs.size(); ++i)
	{
		GatherGameObjects(go->childs[i], gos_vec);
	}
}


