#include "ComponentCanvas.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"

#include "ComponentText.h"
#include "ComponentImage.h"
//#include "Button.h"
//#include "CheckBox.h"
//#include "InputText.h"
//#include "ProgressBar.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentCanvas::ComponentCanvas(GameObject* gameObject) : Component(gameObject, Component::ComponentType::ComponentCanvas)
{
	visible = true;
	interactable = false;
	draggable = false;

	//texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "Canvas Texture");

	App->ui_system->AddCanvas(this);
}

ComponentCanvas::~ComponentCanvas()
{
	App->ui_system->RemoveCanvas(this);
}

void ComponentCanvas::Draw() const
{
	//draw canvas texture

	// Draw elements inside canvas
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i]->GetType() == Component::ComponentType::ComponentCanvas)
		{
			ComponentCanvas* canvas = (ComponentCanvas*)elements[i];
			if (canvas->visible)
				canvas->Draw();
			continue;
		}
		else if (elements[i]->GetType() == Component::ComponentType::ComponentText)
		{
			ComponentText* text = (ComponentText*)elements[i];
			if (text->visible)
				text->Draw();
			continue;
		}
		else if (elements[i]->GetType() == Component::ComponentType::ComponentImage)
		{
			ComponentImage* image = (ComponentImage*)elements[i];
			if (image->visible)
				image->Draw();
			continue;
		}
		//else if (elements[i]->GetType() == Component::ComponentType::Button)
		//{
		//	Button* elem = (Button*)elements[i];
		//	if (elem->visible) 
		//		elem->Draw();
		//}
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

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		GO->RemoveComponent(Component::ComponentType::ComponentCanvas);
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
