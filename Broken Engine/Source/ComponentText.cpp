#include "ComponentText.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentText::ComponentText(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Text)
{
	visible = true;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	//texture = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);
	canvas->AddElement(this);

	//LoadFont("Assets/Fonts/Dukas.ttf", font_size);
	//font.path = "Assets/Fonts/Dukas.ttf";
	//canvas->AddElement(this);

	position2D.y = 150;
}


ComponentText::~ComponentText()
{
	//font.clean();
}

void ComponentText::LoadFont(const char* path, int size)
{
	//font.init(path, size /* size */);
}

void ComponentText::Draw()
{
	glColor4f(color.r, color.g, color.b, color.a);
	//glfreetype::print(camera, font, position2D.x, position2D.y, text);
}

json ComponentText::Save() const
{
	json node;
	return node;
}

void ComponentText::Load(json& node)
{
}

void ComponentText::CreateInspectorNode()
{
	ImGui::Checkbox("##TextActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Text"))
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);
		ImGui::Separator();

		ImGui::ColorEdit3("Color", (float*)&color);

		if (ImGui::DragFloat("Font size", &font_size, 1.0f, 0.0f, 100.0f, "%.2f")) {

			//font.clean();
			//font.init(font.path, font_size);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Use with caution, may temporary freeze the editor with large numbers. \n It is recommended to directly input the number with the keyboard");


		ImGui::Text(text);

		if (ImGui::InputTextWithHint("TextChange", text, buffer, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			strcpy_s(text, buffer);

		}

		// Size
		ImGui::Text("Size:    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##textsize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##textsize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##textposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##textposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##textrotation", &rotation2D);

		// Image
		ImGui::Separator();
		ImGui::Text("Font: ");
		//ImGui::Text(font.path);
		ImGui::SameLine();

		if (ImGui::Button("Load..."))
			ImGui::OpenPopup("Load Font");

		if (ImGui::BeginPopup("Load Font"))
		{
			//if (ImGui::Selectable("Dukas")) {
			//	font.clean();
			//	LoadFont("Assets/Fonts/Dukas.ttf", DEFAULT_FONT_SIZE);
			//	font.path = "Assets/Fonts/Dukas.ttf";
			//}
			//if (ImGui::Selectable("Wintersoul")) {
			//	font.clean();
			//	LoadFont("Assets/Fonts/Wintersoul.ttf", DEFAULT_FONT_SIZE);
			//	font.path = "Assets/Fonts/Wintersoul.ttf";
			//}
			//if (ImGui::Selectable("EvilEmpire")) {
			//	font.clean();
			//	LoadFont("Assets/Fonts/EvilEmpire.otf", DEFAULT_FONT_SIZE);
			//	font.path = "Assets/Fonts/EvilEmpire.otf";
			//}
			//if (ImGui::Selectable("Smack")) {
			//	font.clean();
			//	LoadFont("Assets/Fonts/Smack.otf", DEFAULT_FONT_SIZE);
			//	font.path = "Assets/Fonts/Smack.otf";
			//}
			ImGui::EndPopup();
		}

		//if (!font.textures.empty())
		//	ImGui::Image((ImTextureID)font.textures.back(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		//else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "*Font not loaded*");

		ImGui::Separator();
		ImGui::Separator();
		ImGui::TreePop();
	}

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		GO->RemoveComponent(Component::ComponentType::Text);
	}
}