#include "ComponentButton.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleTextures.h"
#include "PanelScene.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneManager.h"
#include "ModuleFileSystem.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"
//#include "ModuleWindow.h"

#include "ResourceShader.h"
#include "ResourceTexture.h"

#include "Math.h"
#include "ResourceMesh.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentButton::ComponentButton(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Button)
{
	visible = true;
	interactable = true;
	draggable = false;

	collider = { 0,0,0,0 };
	color = idle_color;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "DefaultTexture");

	//font.init("Assets/Fonts/Dukas.ttf", font_size);
	//font.path = "Assets/Fonts/Dukas.ttf";

	canvas->AddElement(this);
}

ComponentButton::~ComponentButton()
{
	if (texture)
	texture->Release();
}

void ComponentButton::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentButton::Draw()
{
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

	//glColorColorF(text_color);
	//glfreetype::print(camera, font, position2D.x + text_pos.x, position2D.y + text_pos.y, text);

	 //--- Update color depending on state ---
	if (state == IDLE) ChangeColorTo(idle_color);
	if (state == HOVERED) ChangeColorTo(hovered_color);
	if (state == SELECTED || state == DRAGGING) ChangeColorTo(selected_color);
	if (state == LOCKED) ChangeColorTo(locked_color);
}

json ComponentButton::Save() const
{
	json node;

	node["Resources"]["ResourceTexture"];

	if (texture)
		node["Resources"]["ResourceTexture"] = std::string(texture->GetResourceFile());

	node["position2Dx"] = std::to_string(position2D.x);
	node["position2Dy"] = std::to_string(position2D.y);

	node["size2Dx"] = std::to_string(size2D.x);
	node["size2Dy"] = std::to_string(size2D.y);

	node["colliderX"] = std::to_string(collider.x);
	node["colliderY"] = std::to_string(collider.y);
	node["colliderW"] = std::to_string(collider.w);
	node["colliderH"] = std::to_string(collider.h);

	node["State"] = std::to_string(state);

	return node;
}

void ComponentButton::Load(json& node)
{
	std::string path = node["Resources"]["ResourceTexture"].is_null() ? "0" : node["Resources"]["ResourceTexture"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	texture = (ResourceTexture*)App->resources->GetResource(std::stoi(path));

	if (texture)
		texture->AddUser(GO);

	std::string position2Dx = node["position2Dx"].is_null() ? "0" : node["position2Dx"];
	std::string position2Dy = node["position2Dy"].is_null() ? "0" : node["position2Dy"];

	std::string size2Dx = node["size2Dx"].is_null() ? "0" : node["size2Dx"];
	std::string size2Dy = node["size2Dy"].is_null() ? "0" : node["size2Dy"];

	std::string colliderX = node["colliderX"].is_null() ? "0" : node["colliderX"];
	std::string colliderY = node["colliderY"].is_null() ? "0" : node["colliderY"];
	std::string colliderW = node["colliderW"].is_null() ? "0" : node["colliderW"];
	std::string colliderH = node["colliderH"].is_null() ? "0" : node["colliderH"];

	std::string state_str = node["State"].is_null() ? "0" : node["State"];

	position2D = float2(std::stof(position2Dx), std::stof(position2Dy));
	size2D = float2(std::stof(size2Dx), std::stof(size2Dy));
	float4 collider_bfr = float4(std::stof(colliderX), std::stof(colliderY), std::stof(colliderW), std::stof(colliderH));
	collider = { collider_bfr.x, collider_bfr.y, collider_bfr.z, collider_bfr.w };
	state = State(std::stoi(state_str));
}

void ComponentButton::CreateInspectorNode()
{
	ImGui::Checkbox("##ImageActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Button"))
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
		ImGui::DragFloat("x##buttonsize", &size2D.x, 0.01f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##buttonsize", &size2D.y, 0.01f);

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

		if (texture == nullptr)
			ImGui::Image((ImTextureID)App->textures->GetDefaultTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //default texture
		else
			ImGui::Image((ImTextureID)texture->GetTexID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)); //loaded texture

		//drag and drop
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
			{
				uint UID = *(const uint*)payload->Data;
				Resource* resource = App->resources->GetResource(UID, false);

				if (resource && resource->GetType() == Resource::ResourceType::TEXTURE)
				{
					if (texture)
						texture->Release();

					texture = (ResourceTexture*)App->resources->GetResource(UID);
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
}

void ComponentButton::UpdateState()
{
	if (interactable == true && visible == true)
	{
		if (state != DRAGGING)
		{
			if (App->ui_system->CheckMousePos(this, collider)) //check if hovering
			{
				ChangeStateTo(HOVERED);
				if (App->ui_system->CheckClick(this, draggable)) //if hovering check if click
				{
					if (draggable == true && (App->ui_system->drag_start.x != App->ui_system->mouse_pos.x || App->ui_system->drag_start.y != App->ui_system->mouse_pos.y)) //if draggable and mouse moves
					{
						ChangeStateTo(DRAGGING);
						position2D.x = App->input->GetMouseX();
						position2D.y = App->input->GetMouseY();
					}
					else
					{
						if (state != SELECTED) //On click action
							OnClick();

						ChangeStateTo(SELECTED);
					}
				}
			}
			else
				ChangeStateTo(IDLE); //if stop hovering
		}
		else
		{
			if (!App->ui_system->CheckClick(this, draggable)) //if stop clicking
				ChangeStateTo(IDLE);
		}
	}
}

void ComponentButton::UpdateCollider()
{
	collider.x = position2D.x - size2D.x; //global pos x
	collider.y = position2D.y - size2D.y; //global pos y
	collider.w = size2D.x * 2; //real width
	collider.h = size2D.y * 2; //real height
}

void ComponentButton::OnClick()
{

}