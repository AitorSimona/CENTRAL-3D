#include "Canvas.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

Canvas::Canvas(GameObject* gameObject, UI_Element::Type type) : UI_Element(gameObject, UI_Element::Type::CANVAS)
{
	visible = true;
	interactable = false;
	draggable = false;

	//texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);
}

Canvas::~Canvas()
{
}

void Canvas::Draw()
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i]->visible)
			elements[i]->Draw();
	}
}

void Canvas::CreateInspectorNode()
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
		GO->RemoveComponent(Component::ComponentType::Mesh);
	}
}
