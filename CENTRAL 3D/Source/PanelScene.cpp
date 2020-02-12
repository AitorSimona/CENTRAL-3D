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
				Resource* resource = App->resources->GetResource(UID);

				// MYTODO: Instance resource here, put it on scene (depending on resource)
				if(resource->GetType() == Resource::ResourceType::MODEL)
				App->resources->GetImporter<ImporterModel>()->InstanceOnCurrentScene(resource->GetResourceFile(), (ResourceModel*)resource);
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
	if (ImGuizmo::IsUsing() == false)
		App->camera->UpdateCamera();
	
	ImGui::End();

	return true;
}

void PanelScene::HandleGuizmo()
{
	// --- Set Current Guizmo operation ---
	if (ImGui::IsWindowHovered())
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
	memcpy(modelMatrix, selectedGO->GetComponent<ComponentTransform>()->GetGlobalTransform().Transposed().ptr(), 16 * sizeof(float));

	// --- Process guizmo operation ---
	ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD; // or Local ??
	ImGuizmo::Manipulate(App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr(), App->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr(), guizmoOperation, mode, modelMatrix);

	// --- Update Selected go transform ---
	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(modelMatrix);
		selectedGO->GetComponent<ComponentTransform>()->SetGlobalTransform(newTransform.Transposed());
	}
}

