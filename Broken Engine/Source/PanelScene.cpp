#include "PanelScene.h"
#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "ImporterScene.h"
#include "ImporterModel.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"

#include "PanelProject.h"

#include "ComponentCamera.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

PanelScene::PanelScene(char * name) : Panel(name)
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
		width = ImGui::GetWindowWidth()*0.98;
		height = ImGui::GetWindowHeight()*0.90;
		ImVec2 size = ImVec2(width, height);

		// --- Force Window Size ---
		if (ImGui::GetWindowWidth() < ImGui::GetWindowHeight())
		{
			size.x = size.y;
			width = height;
			ImGui::SetWindowSize(name, size);
		}
		// MYTODO: limit win size
		// DOCKING HAS NO SUPPORT FOR WINDOW SIZE CONSTRAINTS :(

		if(width > height)
			App->renderer3D->active_camera->SetAspectRatio(width / height);
		else
			App->renderer3D->active_camera->SetAspectRatio(height / width);

		ImGui::Image((ImTextureID)App->renderer3D->rendertexture, size, ImVec2(0, 1), ImVec2(1, 0));

		// --- Save Image's current position (screen space)
		posX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
		posY = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;


		// --- Handle drag & drop ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Resource* resource = App->resources->GetResource(UID, false);

				// MYTODO: Instance resource here, put it on scene (depending on resource)
				if (resource && resource->GetType() == Resource::ResourceType::MODEL)
				{
					resource = App->resources->GetResource(UID);
					App->resources->GetImporter<ImporterModel>()->InstanceOnCurrentScene(resource->GetResourceFile(), (ResourceModel*)resource);
				}
			}

			ImGui::EndDragDropTarget();
		}

		// --- Process input and update editor camera ---
		if (ImGui::IsWindowHovered())
			SceneHovered = true;
		else
			SceneHovered = false;


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("DebugDraw"))
			{
				ImGui::MenuItem("LIGHTING", NULL, &App->renderer3D->lighting);
				ImGui::MenuItem("COLOR MATERIAL", NULL, &App->renderer3D->color_material);
				ImGui::MenuItem("WIREFRAME", NULL, &App->renderer3D->wireframe);
				ImGui::MenuItem("BOUNDING BOXES", NULL, &App->scene_manager->display_boundingboxes);
				ImGui::MenuItem("OCTREE", NULL, &App->scene_manager->display_tree);
				ImGui::MenuItem("ZDRAWER", NULL, &App->renderer3D->zdrawer);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}

	// --- Handle Guizmo operations ---
	if(App->scene_manager->GetSelectedGameObject() != nullptr)
		HandleGuizmo();

	// --- Update editor camera ---
	if (!ImGuizmo::IsUsing())
		App->camera->UpdateCamera();


	if (App->camera->m_ScrollingSpeedChange)
	{
		CurrentSpeedScrollLabel = 0.75;
		App->camera->m_ScrollingSpeedChange = false;
	}

	if (CurrentSpeedScrollLabel > 0)
	{
		bool open = true;

		ImVec2 textSize = ImGui::CalcTextSize(" xf.2f", nullptr);

		ImGui::SetNextWindowBgAlpha(CurrentSpeedScrollLabel);
		ImGui::SetNextWindowPos({ posX + (width / 2.0f) - (textSize.x / 2.0f + 50.0f), posY + (height / 2.0f) - (textSize.y / 2.0f + 50.0f) });
		CurrentSpeedScrollLabel -= 0.015f;
		
		if (ImGui::Begin("Example: Simple overlay", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) //(corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::SetWindowFontScale(3);
			ImGui::Text(" x%.2f", App->camera->m_SpeedMultiplicator);
		}
		
		ImGui::End();
	}
	
	App->gui->sceneHeight = height;
	App->gui->sceneWidth = width;
	App->gui->sceneX = posX;
	App->gui->sceneY = posY;
	App->gui->hoveringScene = SceneHovered;

	ImGui::End();

	return true;
}

void PanelScene::HandleGuizmo()
{
	// --- Set Current Guizmo operation ---
	if (ImGui::IsWindowHovered() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			guizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	GameObject* selectedGO = App->scene_manager->GetSelectedGameObject();

	// --- Set drawing to this window and rendering rect (Scene Image) ---
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(posX, posY, width, height);

	// --- Create temporal matrix to store results of guizmo operations ---
	float modelMatrix[16];
	memcpy(modelMatrix, selectedGO->GetComponent<ComponentTransform>()->GetLocalTransform().Transposed().ptr(), 16 * sizeof(float));

	// --- Process guizmo operation ---
	ImGuizmo::MODE mode = ImGuizmo::MODE::LOCAL; // or Local ??
	ImGuizmo::Manipulate(App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr(), App->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr(), guizmoOperation, mode, modelMatrix);

	// --- Update Selected go transform ---
	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(modelMatrix);
		selectedGO->GetComponent<ComponentTransform>()->SetGlobalTransform(newTransform.Transposed());
	}
}

