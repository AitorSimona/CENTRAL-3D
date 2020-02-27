#include "PanelScene.h"
#include "Imgui/imgui.h"

#include "EngineApplication.h"
#include "PanelProject.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

PanelScene::PanelScene(char* name) : BrokenEngine::Panel(name)
{
	ImGuizmo::Enable(true);
}

PanelScene::~PanelScene()
{
}

// MYTODO: Clean this

bool PanelScene::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		// --- Set image size
		EngineApp->gui->sceneWidth = ImGui::GetWindowWidth()*0.98;
		EngineApp->gui->sceneHeight = ImGui::GetWindowHeight()*0.90;
		ImVec2 size = ImVec2(EngineApp->gui->sceneWidth, EngineApp->gui->sceneHeight);

		// --- Force Window Size ---
		if (ImGui::GetWindowWidth() < ImGui::GetWindowHeight())
		{
			size.x = size.y;
			EngineApp->gui->sceneWidth = EngineApp->gui->sceneHeight;
			ImGui::SetWindowSize(name, size);
		}
		// MYTODO: limit win size
		// DOCKING HAS NO SUPPORT FOR WINDOW SIZE CONSTRAINTS :(

		if(EngineApp->gui->sceneWidth > EngineApp->gui->sceneHeight)
			EngineApp->renderer3D->active_camera->SetAspectRatio(EngineApp->gui->sceneWidth / EngineApp->gui->sceneHeight);
		else
			EngineApp->renderer3D->active_camera->SetAspectRatio(EngineApp->gui->sceneHeight / EngineApp->gui->sceneWidth);

		ImGui::Image((ImTextureID)EngineApp->renderer3D->rendertexture, size, ImVec2(0, 1), ImVec2(1, 0));

		// --- Save Image's current position (screen space)
		EngineApp->gui->sceneX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
		EngineApp->gui->sceneY = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;


		// --- Handle drag & drop ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				BrokenEngine::Resource* resource = EngineApp->resources->GetResource(UID, false);

				// MYTODO: Instance resource here, put it on scene (depending on resource)
				if (resource && resource->GetType() == BrokenEngine::Resource::ResourceType::MODEL)
				{
					resource = EngineApp->resources->GetResource(UID);
					EngineApp->resources->GetImporter < BrokenEngine::ImporterModel > ()->InstanceOnCurrentScene(resource->GetResourceFile(), (BrokenEngine::ResourceModel*)resource);
				}
			}

			ImGui::EndDragDropTarget();
		}

		// --- Process input and update editor camera ---
		EngineApp->gui->isSceneHovered = ImGui::IsWindowHovered();


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("DebugDraw"))
			{
				ImGui::MenuItem("LIGHTING", NULL, &EngineApp->renderer3D->lighting);
				ImGui::MenuItem("COLOR MATERIAL", NULL, &EngineApp->renderer3D->color_material);
				ImGui::MenuItem("WIREFRAME", NULL, &EngineApp->renderer3D->wireframe);
				ImGui::MenuItem("BOUNDING BOXES", NULL, &EngineApp->scene_manager->display_boundingboxes);
				ImGui::MenuItem("OCTREE", NULL, &EngineApp->scene_manager->display_tree);
				ImGui::MenuItem("ZDRAWER", NULL, &EngineApp->renderer3D->zdrawer);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}

	// --- Handle Guizmo operations ---
	if(EngineApp->scene_manager->GetSelectedGameObject() != nullptr)
	HandleGuizmo();

	// --- Update editor camera ---
	if (ImGuizmo::IsUsing() == false)
		EngineApp->camera->UpdateCamera();


	if (EngineApp->camera->m_ScrollingSpeedChange)
	{
		CurrentSpeedScrollLabel = 0.75;
		EngineApp->camera->m_ScrollingSpeedChange = false;
	}

	if (CurrentSpeedScrollLabel > 0)
	{
		bool open = true;
	//	ImVec2 CursorPos(EngineApp->input->GetMouseX() + (SizeGame.x / 2), EngineApp->window->cursorPositionY + (SizeGame.y / 2));
		
		ImGui::SetNextWindowBgAlpha(CurrentSpeedScrollLabel);
		//ImGui::SetNextWindowPos(CursorPos);
		ImGui::SetNextWindowPos({ 600.0f, 250.0f });
		CurrentSpeedScrollLabel -= 0.015f;
		
		if (ImGui::Begin("Example: Simple overlay", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) //(corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::SetWindowFontScale(3);
			ImGui::Text(" x%.2f", EngineApp->camera->m_SpeedMultiplicator);
		}
		
		ImGui::End();
	}
	
	ImGui::End();

	return true;
}

void PanelScene::HandleGuizmo()
{
	// --- Set Current Guizmo operation ---
	if (ImGui::IsWindowHovered() && EngineApp->input->GetMouseButton(SDL_BUTTON_RIGHT) == BrokenEngine::KEY_IDLE)
	{
		if (EngineApp->input->GetKey(SDL_SCANCODE_W) == BrokenEngine::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (EngineApp->input->GetKey(SDL_SCANCODE_E) == BrokenEngine::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (EngineApp->input->GetKey(SDL_SCANCODE_R) == BrokenEngine::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	BrokenEngine::GameObject* selectedGO = EngineApp->scene_manager->GetSelectedGameObject();

	// --- Set drawing to this window and rendering rect (Scene Image) ---
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(EngineApp->gui->sceneX, EngineApp->gui->sceneY, EngineApp->gui->sceneWidth, EngineApp->gui->sceneHeight);

	// --- Create temporal matrix to store results of guizmo operations ---
	float modelMatrix[16];
	memcpy(modelMatrix, selectedGO->GetComponent<BrokenEngine::ComponentTransform>()->GetGlobalTransform().Transposed().ptr(), 16 * sizeof(float));

	// --- Process guizmo operation ---
	ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD; // or Local ??
	ImGuizmo::Manipulate(EngineApp->renderer3D->active_camera->GetOpenGLViewMatrix().ptr(), EngineApp->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr(), guizmoOperation, mode, modelMatrix);

	// --- Update Selected go transform ---
	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(modelMatrix);
		selectedGO->GetComponent<BrokenEngine::ComponentTransform>()->SetGlobalTransform(newTransform.Transposed());
	}
}

