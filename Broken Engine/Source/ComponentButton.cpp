#include "ComponentButton.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"


#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentButton::ComponentButton(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Button)
{
	visible = true;
	interactable = true;
	draggable = false;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	img_texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "DefaultTexture");

	//font.init("Assets/Fonts/Dukas.ttf", font_size);
	//font.path = "Assets/Fonts/Dukas.ttf";

	canvas->AddElement(this);
}

ComponentButton::~ComponentButton()
{
	if (img_texture)
		img_texture->Release();

	if (txt_texture)
		txt_texture->Release();
}

void ComponentButton::Draw()
{
	//glPushMatrix();
	//glLoadIdentity();

	//glTranslatef(position2D.x, position2D.y, 1);
	//glMultTransposeMatrixf(camera->origin_view_matrix);

	//glColorColorF(color);

	//glBindTexture(GL_TEXTURE_2D, material->tex_id);
	//glEnable(GL_TEXTURE_2D);
	//glBegin(GL_QUADS);
	//// Draw A  textured Quad
	//glTexCoord2i(0, 0); glVertex2f(-size2D.x, -size2D.y);    // Top Left		glVertex2i(100, 100);
	//glTexCoord2i(0, 1); glVertex2f(-size2D.x, size2D.y);    // Top Right		glVertex2i(100, 500);
	//glTexCoord2i(1, 1); glVertex2f(size2D.x, size2D.y);    // Bottom Right	glVertex2i(500, 500);
	//glTexCoord2i(1, 0); glVertex2f(size2D.x, -size2D.y);    // Bottom Left	glVertex2i(500, 100);

	//glDisable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glEnd();

	//glPopMatrix();

	//glColorColorF(text_color);
	//glfreetype::print(camera, font, position2D.x + text_pos.x, position2D.y + text_pos.y, text);

	//if (state == IDLE) ChangeColor(idle_color);
	//if (state == HOVERED) ChangeColor(hovered_color);
	//if (state == SELECTED || state == DRAGGING) ChangeColor(selected_color);
	//if (state == LOCKED) ChangeColor(locked_color);
}

json ComponentButton::Save() const
{
	json node;
	return node;
}

void ComponentButton::Load(json& node)
{
}

void ComponentButton::CreateInspectorNode()
{
	ImGui::Checkbox("##ImageActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Button"))
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
		ImGui::DragFloat("x##buttonsize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttonsize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##buttonposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttonposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##buttonrotation", &rotation2D);

		// ------------------------------------------

		// Image
		ImGui::Separator();
		ImGui::Text("Image");

		if (img_texture == nullptr)
			ImGui::Image((ImTextureID)App->textures->GetDefaultTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //default texture
		else
			ImGui::Image((ImTextureID)img_texture->GetTexID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //loaded texture

		//drag and drop
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Resource* resource = App->resources->GetResource(UID, false);

				if (resource && resource->GetType() == Resource::ResourceType::TEXTURE)
				{
					if (img_texture)
						img_texture->Release();

					img_texture = (ResourceTexture*)App->resources->GetResource(UID);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// States (Colors)
		ImGui::Separator();
		ImGui::ColorEdit4("##Idle", (float*)&idle_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Idle");

		ImGui::ColorEdit4("##Hovered", (float*)&hovered_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Hovered");

		ImGui::ColorEdit4("##Selected", (float*)&selected_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Selected");

		ImGui::ColorEdit4("##Locked", (float*)&locked_color, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Locked");

		//// Text
		//ImGui::Separator();
		//if (ImGui::DragFloat("Font size", &font_size, 1.0f, 0.0f, 100.0f, "%.2f")) {

		//	font.clean();
		//	font.init(font.path, font_size);
		//}
		//if (ImGui::IsItemHovered())
		//	ImGui::SetTooltip("Use with caution, may temporary freeze the editor with large numbers. \n It is recommended to directly input the number with the keyboard");
		//if (ImGui::Button("Load font..."))
		//	ImGui::OpenPopup("Load Font");

		//if (ImGui::BeginPopup("Load Font"))
		//{
		//	if (ImGui::Selectable("Dukas")) {
		//		font.clean();
		//		font.init("Assets/Fonts/Dukas.ttf", font_size);
		//		font.path = "Assets/Fonts/Dukas.ttf";
		//	}
		//	if (ImGui::Selectable("Wintersoul")) {
		//		font.clean();
		//		font.init("Assets/Fonts/Wintersoul.ttf", font_size);
		//		font.path = "Assets/Fonts/Wintersoul.ttf";
		//	}
		//	if (ImGui::Selectable("EvilEmpire")) {
		//		font.clean();
		//		font.init("Assets/Fonts/EvilEmpire.otf", font_size);
		//		font.path = "Assets/Fonts/EvilEmpire.otf";
		//	}
		//	if (ImGui::Selectable("Smack")) {
		//		font.clean();
		//		font.init("Assets/Fonts/Smack.otf", font_size);
		//		font.path = "Assets/Fonts/Smack.otf";
		//	}
		//	ImGui::EndPopup();
		//}
		//ImGui::Text("Text");
		//ImGui::InputText("##buttontext", text, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		//ImGui::ColorEdit3("Color", (float*)&text_color);

		//ImGui::Text("Position:");
		//ImGui::SameLine();
		//ImGui::SetNextItemWidth(60);
		//ImGui::DragFloat("x##buttontextposition", &text_pos.x);
		//ImGui::SameLine();
		//ImGui::SetNextItemWidth(60);
		//ImGui::DragFloat("y##buttontextposition", &text_pos.y);


		// Action
		ImGui::Separator();
		if (ImGui::Button("Action"))
		{
			//drag and drop script
		}

		ImGui::Separator();
		ImGui::Separator();
		ImGui::TreePop();
	}

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		GO->RemoveComponent(Component::ComponentType::Button);
	}
}