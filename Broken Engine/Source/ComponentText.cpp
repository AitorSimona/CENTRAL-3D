#include "ComponentText.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ComponentCanvas.h"
#include "ModuleRenderer3D.h"
#include "ResourceShader.h"
#include "ComponentCamera.h"
#include "ResourceFont.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentText::ComponentText(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Text)
{
	name = "Text";
	visible = true;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	canvas->AddElement(this);

	font = App->resources->DefaultFont;
	if (font)
		font->AddUser(GO);
}


ComponentText::~ComponentText() 
{

	if (font && font->IsInMemory())
	{
		font->Release();
		font->RemoveUser(GO);
	}
}

void ComponentText::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentText::Draw()
{
	if (font == nullptr) {
		//WARNING LOG("No font available in text component");
		return;
	}

	// --- Update transform and rotation to face camera ---
	float3 frustum_pos = App->renderer3D->active_camera->frustum.Pos();
	float3 center = float3(frustum_pos.x, frustum_pos.y, 10);

	// --- Frame image with camera ---
	float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, 10),
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
		float3(size2D, 1));

	float3 Movement = App->renderer3D->active_camera->frustum.Front();
	float3 camera_pos = frustum_pos;

	if (Movement.IsFinite())
		App->renderer3D->active_camera->frustum.SetPos(center - Movement);

	// Options
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->textShader->ID);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		position2D.x * 0.01f, position2D.y * 0.01f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->textShader->ID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	glUniform3f(glGetUniformLocation(App->renderer3D->textShader->ID, "textColor"), color.r, color.g, color.b);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(font->VAO);

	float x = position2D.x;
	float y = position2D.y;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		ResourceFont::Character ch = font->characters[*c];

		GLfloat xpos = x + ch.Bearing.x * size2D.x;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * size2D.y;

		GLfloat w = ch.Size.x * size2D.x;
		GLfloat h = ch.Size.y * size2D.y;

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
		glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * size2D.x; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(App->renderer3D->defaultShader->ID);

	// --- Set camera back to original position ---
	App->renderer3D->active_camera->frustum.SetPos(camera_pos);
}

json ComponentText::Save() const
{
	json node;
	node["Active"] = this->active;
	node["visible"] = std::to_string(visible);

	node["position2Dx"] = std::to_string(position2D.x);
	node["position2Dy"] = std::to_string(position2D.y);

	node["rotation2D"] = std::to_string(rotation2D);

	node["scale2Dx"] = std::to_string(size2D.x);
	node["scale2Dy"] = std::to_string(size2D.y);

	node["colorR"] = std::to_string(color.r);
	node["colorG"] = std::to_string(color.g);
	node["colorB"] = std::to_string(color.b);
	node["colorA"] = std::to_string(color.a);

	node["text"] = text;

	if (font) 
		node["font"] = std::to_string(font->GetUID());
	

	return node;
}

void ComponentText::Load(json& node)
{
	//When loading scene, load the saved font if exists else use default font

	this->active = node["Active"].is_null() ? true : (bool)node["Active"];
	std::string visible_str = node["visible"].is_null() ? "0" : node["visible"];
	visible = bool(std::stoi(visible_str));

	std::string posx = node["position2Dx"].is_null() ? "0" : node["position2Dx"];
	std::string posy = node["position2Dy"].is_null() ? "0" : node["position2Dy"];

	std::string rot = node["rotation2D"].is_null() ? "0" : node["rotation2D"];

	std::string scalex = node["scale2Dx"].is_null() ? "0" : node["scale2Dx"];
	std::string scaley = node["scale2Dy"].is_null() ? "0" : node["scale2Dy"];

	std::string colorr = node["colorR"].is_null() ? "1" : node["colorR"];
	std::string colorg = node["colorG"].is_null() ? "1" : node["colorG"];
	std::string colorb = node["colorB"].is_null() ? "1" : node["colorB"];
	std::string colora = node["colorA"].is_null() ? "1" : node["colorA"];

	std::string Text = node["text"].is_null() ? "ErrorLoading" : node["text"];
	std::string Font = node["font"].is_null() ? std::to_string(App->resources->DefaultFont->GetUID()).c_str() : node["font"];
	
	Move({ std::stof(posx),std::stof(posy) });
	Rotate(std::stof(rot));
	Scale({ std::stof(scalex), std::stof(scaley) });

	color = { std::stof(colorr),std::stof(colorg),std::stof(colorb),std::stof(colora) };

	text = Text;

	font = (ResourceFont*)App->resources->GetResource(std::stoul(Font));
}

// Function called before dying
void ComponentText::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (font && UID == font->GetUID())
		{
			font = nullptr;
		}
		break;

	default:
		break;
	}
}
void ComponentText::CreateInspectorNode()
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	ImGui::Checkbox("Visible", &visible);
	ImGui::Separator();

	ImGui::ColorEdit3("Color", (float*)&color);

	// Temporary disabled for usability and design consistency
	/*if (ImGui::DragInt("Font size", &font->size, 1, 1, 200, "%.2f")) {

		font->Init();
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Use with caution, may temporary freeze the editor with large numbers. \n It is recommended to directly input the number with the keyboard");
	*/

	ImGui::Text(text.c_str());

	if (ImGui::InputTextWithHint("TextChange", text.c_str(), buffer, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		//strcpy_s(text, buffer);
		text = buffer;
	}

	// Size
	ImGui::Text("Size:    ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("x##textsize", &size2D.x, 0.001f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("y##textsize", &size2D.y, 0.001f);

	// Position
	ImGui::Text("Position:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("x##textposition", &position2D.x, 0.05f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("y##textposition", &position2D.y, 0.05f);

	// Rotation
	//ImGui::Text("Rotation:");
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(60);
	//ImGui::DragFloat("##textrotation", &rotation2D);

	// Image
	ImGui::Separator();
	ImGui::Text("Font: ");
	ImGui::SameLine();


	// --- Texture Preview ---
	if (font) {
		ImGui::Text(font->GetName());
		ImGui::ImageButton((void*)(uint)font->GetPreviewTexID(), ImVec2(20, 20));
	}
	else
		ImGui::ImageButton(NULL, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 2);

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Drag and drop any font from your assets to change it");

	// --- Handle drag & drop ---
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("resource")) {
			uint UID = *(const uint*)payload->Data;
			Resource* resource = App->resources->GetResource(UID, false);

			if (resource && resource->GetType() == Resource::ResourceType::FONT) {
				if (font && font->IsInMemory())
				{
					font->Release();
					font->RemoveUser(GO);
				}

				font = (ResourceFont*)App->resources->GetResource(UID);

				if (font)
					font->AddUser(GO);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::Separator();

}