#include "ComponentCanvas.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleTextures.h"
#include "PanelScene.h"
#include "ModuleRenderer3D.h"

#include "ComponentText.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
//#include "ComponentCheckBox.h"
//#include "ComponentInputText.h"
//#include "ComponentProgressBar.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentCanvas::ComponentCanvas(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Canvas)
{
	visible = true;
	interactable = false;
	draggable = false;

	texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "Canvas Texture");

	App->ui_system->AddCanvas(this);
}

ComponentCanvas::~ComponentCanvas()
{
	App->ui_system->RemoveCanvas(this);
}

void ComponentCanvas::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentCanvas::Draw() const
{
	// Draw canvas texture
		// --- Update transform and rotation to face camera ---
	float3 frustum_pos = App->renderer3D->active_camera->frustum.Pos();
	float3 center = float3(frustum_pos.x, frustum_pos.y, 10);

	// --- Frame image with camera ---
	float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, 10),
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
		float3(size2D * 0.01f, 1));

	float3 Movement = App->renderer3D->active_camera->frustum.Front();
	float3 camera_pos = frustum_pos;

	if (Movement.IsFinite())
		App->renderer3D->active_camera->frustum.SetPos(center - Movement);

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->defaultShader->ID);

	// color tint
	int TextureLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
	glUniform1i(TextureLocation, -1);
	GLint vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
	glUniform3f(vertexColorLocation, color.r, color.g, color.b);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		position2D.x * 0.01f, position2D.y * 0.01f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	glUniform1i(TextureLocation, 0); //reset texture location

	// --- Draw plane with given texture ---
	glBindVertexArray(App->scene_manager->plane->VAO);

	glBindTexture(GL_TEXTURE_2D, texture->GetTexID());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->plane->EBO);
	glDrawElements(GL_TRIANGLES, App->scene_manager->plane->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)

	// --- Set camera back to original position ---
	App->renderer3D->active_camera->frustum.SetPos(camera_pos);


	// Draw elements inside canvas
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i]->GetType() == Component::ComponentType::Canvas)
		{
			ComponentCanvas* canvas = (ComponentCanvas*)elements[i];
			if (canvas->visible)
				canvas->Draw();
			continue;
		}
		else if (elements[i]->GetType() == Component::ComponentType::Text)
		{
			ComponentText* text = (ComponentText*)elements[i];
			if (text->visible)
				text->Draw();
			continue;
		}
		else if (elements[i]->GetType() == Component::ComponentType::Image)
		{
			ComponentImage* image = (ComponentImage*)elements[i];
			if (image->visible)
				image->Draw();
			continue;
		}
		else if (elements[i]->GetType() == Component::ComponentType::Button)
		{
			ComponentButton* button = (ComponentButton*)elements[i];
			if (button->visible) 
				button->Draw();
		}
		//else if (elements[i]->GetType() == Component::ComponentType::CheckBox)
		//{
		//	CheckBox* elem = (CheckBox*)elements[i];
		//	if (elem->visible) 
		//		elem->Draw();
		//	continue;
		//}
		//else if (elements[i]->GetType() == Component::ComponentType::InputText)
		//{
		//	InputText* elem = (InputText*)elements[i];
		//	if (elem->visible) 
		//		elem->Draw();
		//	continue;
		//}
		//else if (elements[i]->GetType() == Component::ComponentType::ProgressBar)
		//{
		//	ProgressBar* elem = (ProgressBar*)elements[i];
		//	if (elem->visible) 
		//		elem->Draw();
		//	continue;
		//}
		else
			continue;
	}
}

json ComponentCanvas::Save() const
{
	json node;



	return node;
}

void ComponentCanvas::Load(json& node)
{
}

void ComponentCanvas::CreateInspectorNode()
{
	ImGui::Checkbox("##CanvasActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Canvas"))
	{
		if (ImGui::Button("Delete component"))
			to_delete = true;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Interactable", &interactable);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Draggable", &draggable);
		ImGui::Separator();

		// Size
		ImGui::Text("Size:    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##canvassize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##canvassize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##canvasposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##canvasposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##canvasrotation", &rotation2D);

		ImGui::Separator();
		ImGui::Separator();

		ImGui::TreePop();
	}

}

void ComponentCanvas::UpdatePosition()
{
	//for (Component* elem : elements)
	//{
	//	MoveElement()
	//}
}

void ComponentCanvas::UpdateCollider()
{
	collider.x = position2D.x - size2D.x;
	collider.y = position2D.y - size2D.y;
	collider.w = size2D.x * 2;
	collider.h = size2D.y * 2;
}

void ComponentCanvas::UpdateState()
{
	if (interactable && visible)
	{
		if (state != DRAGGING)
		{
			UpdateCollider();
			if (App->ui_system->CheckMousePos(this, collider))
			{
				ChangeStateTo(HOVERED);
				if (App->ui_system->CheckClick(this, draggable))
				{
					if (draggable && (App->ui_system->drag_start.x != App->ui_system->mouse_pos.x || App->ui_system->drag_start.y != App->ui_system->mouse_pos.y))
						ChangeStateTo(DRAGGING);
					else
						ChangeStateTo(SELECTED);
				}
			}
			else
				ChangeStateTo(IDLE);
		}
		else
		{
			if (!App->ui_system->CheckClick(this, draggable))
				ChangeStateTo(IDLE);
		}
	}
}
