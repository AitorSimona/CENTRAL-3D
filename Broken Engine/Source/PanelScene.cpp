#include "PanelScene.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"
#include "ModuleResourceManager.h"
#include "ModuleSceneManager.h"
#include "ModuleSelection.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"

// -- Panels --
#include "PanelProject.h"

// -- Components --
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

// -- Importers --
#include "ImporterModel.h"

// -- Utilities --
#include "OpenGL.h"
#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

PanelScene::PanelScene(char* name) : Panel(name)
{
	ImGuizmo::Enable(true);
	overlay = "Camera Speed Overlay";
}

PanelScene::~PanelScene()
{
}

// MYTODO: Clean this

bool PanelScene::Draw()
{
	EngineApp->gui->draw_list = ImGui::GetForegroundDrawList(); //used for ui system

	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		// --- Set image size
		width = ImGui::GetWindowWidth()*0.98;
		height = ImGui::GetWindowHeight()*0.90;
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
		posX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
		posY = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;


		// --- Handle drag & drop ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Broken::Resource* resource = EngineApp->resources->GetResource(UID, false);

				if (resource->GetType() == Broken::Resource::ResourceType::MODEL)
				{
					resource = EngineApp->resources->GetResource(UID);
					EngineApp->resources->GetImporter<Broken::ImporterModel>()->InstanceOnCurrentScene(resource->GetResourceFile(), (Broken::ResourceModel*)resource);
				}
				if (resource->GetType() == Broken::Resource::ResourceType::PREFAB)
				{
					// We force a model instance without sending model*
					resource = EngineApp->resources->GetResource(UID);
					EngineApp->resources->GetImporter<Broken::ImporterModel>()->InstanceOnCurrentScene(resource->GetResourceFile(), nullptr);
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
				ImGui::MenuItem("BOUNDING BOXES", NULL, &EngineApp->renderer3D->display_boundingboxes);
				ImGui::MenuItem("OCTREE", NULL, &EngineApp->scene_manager->display_tree);
				ImGui::MenuItem("ZDRAWER", NULL, &EngineApp->renderer3D->zdrawer);

				if (ImGui::MenuItem("NORMAL MAPPING", NULL, &EngineApp->renderer3D->m_Draw_normalMapping))
				{
					EngineApp->renderer3D->m_Draw_normalMapping_Lit = false;
					EngineApp->renderer3D->m_Draw_normalMapping_Lit_Adv = false;
				}
				if (ImGui::MenuItem("LIT NORMAL MAPPING", NULL, &EngineApp->renderer3D->m_Draw_normalMapping_Lit))
				{
						EngineApp->renderer3D->m_Draw_normalMapping = false;
						EngineApp->renderer3D->m_Draw_normalMapping_Lit_Adv = false;
				}
				if (ImGui::MenuItem("LIT NORMAL MAPPING ADVANCED", NULL, &EngineApp->renderer3D->m_Draw_normalMapping_Lit_Adv))
				{
					EngineApp->renderer3D->m_Draw_normalMapping_Lit = false;
					EngineApp->renderer3D->m_Draw_normalMapping = false;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		EngineApp->gui->sceneHeight = height;
		EngineApp->gui->sceneWidth = width;
		EngineApp->gui->sceneX = posX;
		EngineApp->gui->sceneY = posY;
		EngineApp->gui->isSceneHovered = ImGui::IsWindowHovered();
	}

	// --- Handle Guizmo operations ---
	// SELECTED TODO
	if(EngineApp->selection->GetLastSelected() != nullptr)
		HandleGuizmo();

	// --- Update editor camera ---
	if (!ImGuizmo::IsUsing())
		EngineApp->camera->UpdateCamera();


	if (EngineApp->camera->m_ScrollingSpeedChange)
	{
		CurrentSpeedScrollLabel = 0.75;
		EngineApp->camera->m_ScrollingSpeedChange = false;
	}

	if (CurrentSpeedScrollLabel > 0)
	{
		ImVec2 textSize = ImGui::CalcTextSize(" xf.2f", nullptr);

		ImGui::SetNextWindowBgAlpha(CurrentSpeedScrollLabel);
		ImGui::SetNextWindowPos({ posX + (width / 2.0f) - (textSize.x / 2.0f + 50.0f), posY + (height / 2.0f) - (textSize.y / 2.0f + 50.0f) });
		CurrentSpeedScrollLabel -= 0.015f;
		
		if (ImGui::Begin(overlay, &overlay_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) //(corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::SetWindowFontScale(2);
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
	if (ImGui::IsWindowHovered() && EngineApp->input->GetMouseButton(SDL_BUTTON_RIGHT) == Broken::KEY_IDLE)
	{
		if (EngineApp->input->GetKey(SDL_SCANCODE_W) == Broken::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (EngineApp->input->GetKey(SDL_SCANCODE_E) == Broken::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (EngineApp->input->GetKey(SDL_SCANCODE_R) == Broken::KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	Broken::ComponentTransform* root_transform = EngineApp->selection->root->GetComponent<Broken::ComponentTransform>();

	// --- Set drawing to this window and rendering rect (Scene Image) ---
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(EngineApp->gui->sceneX, EngineApp->gui->sceneY, EngineApp->gui->sceneWidth, EngineApp->gui->sceneHeight);

	// --- Create temporal matrix to store results of guizmo operations ---
	float modelMatrix[16], deltaMatrix[16];
	memcpy(modelMatrix, root_transform->GetLocalTransform().Transposed().ptr(), 16 * sizeof(float));

	// --- Process guizmo operation ---
	ImGuizmo::MODE mode = ImGuizmo::MODE::LOCAL; // or Local ??
	ImGuizmo::Manipulate(EngineApp->renderer3D->active_camera->GetOpenGLViewMatrix().ptr(), EngineApp->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr(), guizmoOperation, mode, modelMatrix, deltaMatrix);

	// --- Update Selected go transform ---
	if (ImGuizmo::IsUsing())
	{
		float3 delta_pos, delta_rot, delta_scale;
		ImGuizmo::DecomposeMatrixToComponents(deltaMatrix, delta_pos.ptr(), delta_rot.ptr(), delta_scale.ptr());

		EngineApp->selection->UseGuizmo(guizmoOperation, delta_pos, delta_rot, delta_scale);
	}
}

