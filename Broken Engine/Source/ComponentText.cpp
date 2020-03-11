#include "ComponentText.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ComponentCanvas.h"
#include "ResourceTexture.h"
#include "ModuleRenderer3D.h"
#include "ResourceShader.h"
#include "ComponentCamera.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentText::ComponentText(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Text)
{
	visible = true;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	//texture = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);
	canvas->AddElement(this);

	//LoadFont("Assets/Fonts/Dukas.ttf", font_size);
	//font.path = "Assets/Fonts/Dukas.ttf";
	//canvas->AddElement(this);

	position2D.y = 0;
}


ComponentText::~ComponentText()
{
	//font.clean();
}

void ComponentText::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentText::LoadFont(const char* path, int size)
{
	//font.init(path, size /* size */);
}

void ComponentText::Draw()
{
	glColor4f(color.r, color.g, color.b, color.a);
	Print(text, position2D.x, position2D.y, size2D.x, color);
}

void ComponentText::Print(std::string text, float x, float y, float scale, Color color)
{
	// Options
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- Frame image with camera ---
	float4x4 transform = float4x4::identity;

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->textShader->ID);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		position2D.x * 0.01f, position2D.y * 0.01f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	/////////////////////////////////////////
	// Activate corresponding render state	
	//glUseProgram(App->renderer3D->textShader->ID);
	//
	////float4x4 projection = glOrtho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT))
	//GLint loc = glGetUniformLocation(App->renderer3D->textShader->ID, "projection");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr());

	
	glUniform3f( glGetUniformLocation(App->renderer3D->textShader->ID, "textColor"), color.r, color.g, color.b);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(App->ui_system->VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		ModuleUI::Character ch = App->ui_system->characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		// x,y,z ,tex.x, tex.y
		GLfloat vertices[6][5] = {
			{ xpos,     ypos + h,   0.0, 0.0, 0.0 },
			{ xpos,     ypos,       0.0, 0.0, 1.0 },
			{ xpos + w, ypos,       0.0, 1.0, 1.0 },
									 
			{ xpos,     ypos + h,   0.0, 0.0, 0.0 },
			{ xpos + w, ypos,       0.0, 1.0, 1.0 },
			{ xpos + w, ypos + h,   0.0, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, App->ui_system->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(App->renderer3D->defaultShader->ID);
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
		if (ImGui::Button("Delete component"))
			to_delete = true;

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

}