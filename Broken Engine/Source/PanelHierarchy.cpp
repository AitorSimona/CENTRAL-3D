#include "PanelHierarchy.h"
#include "EngineApplication.h"

#include "Imgui/imgui.h"

// -- Modules --
#include "ModuleEditorUI.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleSelection.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleFilesystem.h"

// -- Resources --
#include "ResourcePrefab.h"
#include "ResourceScene.h"

// -- Components
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "ComponentProgressbar.h"
#include "ComponentLight.h"

// -- Importer
#include "ImporterModel.h"
#include "ImporterPrefab.h"

// -- Panels --
#include "PanelProject.h"

// -- Utilities --
#include "EngineLog.h"

PanelHierarchy::PanelHierarchy(char * name) : Panel(name) {}

PanelHierarchy::~PanelHierarchy() {}

bool PanelHierarchy::Draw()
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (EngineApp->gui->editingPrefab)
			{
				if (ImGui::ArrowButton("##Back", ImGuiDir_::ImGuiDir_Left))
				{
					ExitEditPrefab();
				}
				else
				{
					ImGui::Image((ImTextureID)EngineApp->gui->prefabTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					ImGui::Text(EngineApp->gui->prefab->GetName());
				}
			}
			else
			{
				ImGui::Image((ImTextureID)EngineApp->gui->sceneTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::SameLine();
				ImGui::Text(EngineApp->scene_manager->currentScene->GetName());
			}
		}
		ImGui::EndMenuBar();

		// --- Instance opened prefab for editing ---
		if (EngineApp->gui->openPrefab)
		{
			EngineApp->gui->openPrefab = false;

			// --- Deactivate gos temporarily ---
			EngineApp->scene_manager->currentScene->DeactivateAllGameObjects();

			Broken::ImporterModel* IModel = EngineApp->resources->GetImporter<Broken::ImporterModel>();
			EngineApp->gui->prefab->parentgo = IModel->InstanceOnCurrentScene(EngineApp->gui->prefab->GetResourceFile(), nullptr);
			EngineApp->gui->editingPrefab = true;
		}

		if (EngineApp->gui->editingPrefab)
		{
			if(EngineApp->gui->prefab->parentgo)
				DrawRecursive(EngineApp->gui->prefab->parentgo);
		}
		else
			DrawRecursive(EngineApp->scene_manager->GetRootGO());

		// Deselect the current GameObject when clicking in an empty space of the hierarchy
		if (ImGui::InvisibleButton("##Deselect", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - ImGui::GetCursorPosY() }))
			EngineApp->selection->ClearSelection();
			//EngineApp->scene_manager->SetSelectedGameObject(nullptr);

		// Allow creating GameObjects and UI Elements from the hierarchy
		if (EngineApp->input->GetMouseButton(SDL_BUTTON_RIGHT) == Broken::KEY_DOWN
			&& ImGui::GetMousePos().x < ImGui::GetWindowWidth() + ImGui::GetWindowPos().x && ImGui::GetMousePos().x > ImGui::GetWindowPos().x
			&& ImGui::GetMousePos().y < ImGui::GetWindowHeight() + ImGui::GetWindowPos().y && ImGui::GetMousePos().y > ImGui::GetWindowPos().y)
			ImGui::OpenPopup("Create new element");

		if (ImGui::BeginPopup("Create new element")) {
			if (ImGui::BeginMenu("3D Object")) {
				if (ImGui::MenuItem("Empty Game Object")) {
					Broken::GameObject* go = EngineApp->scene_manager->CreateEmptyGameObject();
				}

				if (ImGui::MenuItem("Plane")) {
					Broken::GameObject* obj = EngineApp->scene_manager->LoadPlane();
					obj->GetComponent<Broken::ComponentTransform>()->SetRotation({ -90, 0, 0 });
					obj->GetComponent < Broken::ComponentTransform >()->Scale(10, 10, 10);
				}

				if (ImGui::MenuItem("Cube"))
					EngineApp->scene_manager->LoadCube();

				if (ImGui::MenuItem("Cylinder"))
					EngineApp->scene_manager->LoadCylinder()->GetComponent<Broken::ComponentTransform>()->SetRotation({ -90, 0, 0 });

				if (ImGui::MenuItem("Capsule"))
					EngineApp->scene_manager->LoadCapsule();

				if (ImGui::MenuItem("Sphere"))
					EngineApp->scene_manager->LoadSphere();

				if (ImGui::MenuItem("Camera")) {
					Broken::GameObject* cam = EngineApp->scene_manager->CreateEmptyGameObject();

					Broken::ComponentCamera* camera = (Broken::ComponentCamera*)cam->AddComponent(Broken::Component::ComponentType::Camera);
					cam->AddComponent(Broken::Component::ComponentType::MeshRenderer);
					camera->SetFarPlane(10);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("UI Element")) {
				if (ImGui::MenuItem("Canvas")) {
					Broken::GameObject* canvas_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentCanvas* camera = (Broken::ComponentCanvas*)canvas_go->AddComponent(Broken::Component::ComponentType::Canvas);
				}
				if (ImGui::MenuItem("Image")) {
					Broken::GameObject* image_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentImage* image = (Broken::ComponentImage*)image_go->AddComponent(Broken::Component::ComponentType::Image);
				}
				if (ImGui::MenuItem("Text")) {
					Broken::GameObject* text_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentText* text = (Broken::ComponentText*)text_go->AddComponent(Broken::Component::ComponentType::Text);
				}
				if (ImGui::MenuItem("Button")) {
					Broken::GameObject* button_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentText* button = (Broken::ComponentText*)button_go->AddComponent(Broken::Component::ComponentType::Button);
				}
				if (ImGui::MenuItem("Checkbox")) {

				}
				if (ImGui::MenuItem("Input Text")) {

				}
				if (ImGui::MenuItem("Progress Bar")) {
					Broken::GameObject* bar_go = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentProgressBar* bar = (Broken::ComponentProgressBar*)bar_go->AddComponent(Broken::Component::ComponentType::ProgressBar);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Lights"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::DIRECTIONAL);
				}

				if (ImGui::MenuItem("Pointlight"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::POINTLIGHT);
				}

				if (ImGui::MenuItem("Spotlight"))
				{
					Broken::GameObject* lightGObj = EngineApp->scene_manager->CreateEmptyGameObject();
					Broken::ComponentLight* light = (Broken::ComponentLight*)lightGObj->AddComponent(Broken::Component::ComponentType::Light);
					light->SetLightType(Broken::LightType::SPOTLIGHT);
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}			
	}
	ImGui::End();

	// --- Manage Drag & Drop ---
	if (end_drag)
	{
		bool to_be_cleared = false;

		if (EngineApp->selection->IsSelected(target) == false) 
		{
			for (Broken::GameObject* obj : *EngineApp->selection->GetSelected())
			{
				// Checking infite loops parent-child
				//if (target->FindParentGO(obj) == false && obj->FindChildGO(target) == false)
				if (!(target->FindParentGO(obj) || obj->FindChildGO(target))) // same as line above but more efficient
				{
					target->AddChildGO(obj);
					to_be_cleared = true;
				}
			}
		}
		end_drag = false;
		dragged = nullptr;
		target = nullptr;
		if (to_be_cleared)
			EngineApp->selection->ClearSelection();
	}
	if (to_unparent)
	{
		for (Broken::GameObject* obj : *EngineApp->selection->GetSelected())
		{
			EngineApp->scene_manager->GetRootGO()->AddChildGO(obj);
		}
		EngineApp->selection->ClearSelection();
		to_unparent = false;
	}
	return true;
}

void PanelHierarchy::ExitEditPrefab()
{
	std::string previewTexpath;
	std::vector<Broken::GameObject*> prefab_gos;
	EngineApp->editorui->panelProject->GatherGameObjects(EngineApp->gui->prefab->parentgo, prefab_gos);
	uint texID = 0;
	previewTexpath = EngineApp->renderer3D->RenderSceneToTexture(prefab_gos, texID);

	EngineApp->fs->Remove(EngineApp->gui->prefab->previewTexPath.c_str());
	EngineApp->gui->prefab->previewTexPath = previewTexpath;
	EngineApp->gui->prefab->SetPreviewTexID(texID);

	Broken::ImporterPrefab* IPrefab = EngineApp->resources->GetImporter<Broken::ImporterPrefab>();
	IPrefab->Save(EngineApp->gui->prefab);

	EngineApp->selection->ClearSelection();

	EngineApp->gui->editingPrefab = false;

	if (EngineApp->gui->prefab->parentgo)
		EngineApp->scene_manager->DestroyGameObject(EngineApp->gui->prefab->parentgo);

	EngineApp->gui->prefab->parentgo = nullptr;
	EngineApp->gui->prefab = nullptr;

	EngineApp->scene_manager->currentScene->ActivateAllGameObjects();
}

void PanelHierarchy::DrawRecursive(Broken::GameObject * Go)
{
	// --- Avoid displaying root ---
	if (Go->GetName() == EngineApp->scene_manager->GetRootGO()->GetName())
	{
		if (Go->childs.size() > 0)
		{
			for (std::vector<Broken::GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
			{
				DrawRecursive(*it);
			}
		}
	}

	// --- Display Go node ---
    else 
	{
		if (Go->childs.empty())
			Go->node_flags |= ImGuiTreeNodeFlags_Leaf;
		else
			Go->node_flags &= ~ImGuiTreeNodeFlags_Leaf;

		// --- Create current node and get if it is opened or not ---
		if(!Go->GetActive())
		ImGui::PushStyleColor(ImGuiCol(), ImVec4(0.5, 0.5, 0.5, 1));

		if (Go->is_prefab_child || Go->is_prefab_instance)
		{
			ImGui::Image((ImTextureID)EngineApp->gui->prefabTexID, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::SameLine();
		}

		bool open = ImGui::TreeNodeEx((void*)Go->GetUID(), Go->node_flags, Go->GetName());

		if (!Go->GetActive())
		ImGui::PopStyleColor();

		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			uint UID = Go->GetUID();
			ImGui::SetDragDropPayload("GO", &UID, sizeof(uint));
			dragged = Go;
			ImGui::Text(Go->GetName());
			ImGui::EndDragDropSource();
			//EngineApp->selection->Select(dragged); // Aitor: Should not set dragged as selected since we won't be able to drag one go into another's component!
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GO"))
			{
				target = Go;
				end_drag = true;
			}
			ImGui::EndDragDropTarget();
		}

		// --- Set Game Object to be destroyed ---
		if (ImGui::IsWindowFocused() && EngineApp->selection->IsSelected(Go) && EngineApp->input->GetKey(SDL_SCANCODE_DELETE) == Broken::KEY_DOWN)
		{
			EX_ENGINE_CONSOLE_LOG("Destroying: %s ...",  Go->GetName());
			EngineApp->scene_manager->SendToDelete(Go);
		}

		// --- Handle selection ---
		if (selected_uid == Go->GetUID() && wasclicked && ImGui::IsMouseReleased(0) )
		{
			if (ImGui::IsItemHovered())
			{
				EngineApp->selection->HandleSelection(Go);
				wasclicked = false;
			}
			else
			{
				to_unparent = true;
			}

		}

		// --- Handle selection ---
		if (ImGui::IsItemClicked())
		{
			selected_uid = Go->GetUID();
			wasclicked = true;
		}

		// --- Display children only if current node is open ---
		if (open)
		{
			// --- Check for children and draw them the same way ---
			if (Go->childs.size() > 0)
			{
				for (std::vector<Broken::GameObject*>::iterator it = Go->childs.begin(); it != Go->childs.end(); ++it)
				{
					DrawRecursive(*it);
				}
			}

			ImGui::TreePop();
		}
	}
}
