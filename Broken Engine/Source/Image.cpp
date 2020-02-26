#include "Image.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleTextures.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

Image::Image(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Image)
{
	visible = true;

	canvas = (Canvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "Default");
	texture->SetTextureID(App->textures->GetDefaultTextureID());
	canvas->AddElement(this);
}

Image::~Image()
{
}

void Image::Draw()
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(position2D.x, position2D.y, 1);
	//glMultTransposeMatrixf(camera->origin_view_matrix);


	glBindTexture(GL_TEXTURE_2D, texture->GetTexID());
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	// Draw A Quad
	glTexCoord2i(0, 0); glVertex2f(-size2D.x, -size2D.y);    // Top Left		glVertex2i(100, 100);
	glTexCoord2i(0, 1); glVertex2f(-size2D.x, size2D.y);    // Top Right		glVertex2i(100, 500);
	glTexCoord2i(1, 1); glVertex2f(size2D.x, size2D.y);    // Bottom Right	glVertex2i(500, 500);
	glTexCoord2i(1, 0); glVertex2f(size2D.x, -size2D.y);    // Bottom Left	glVertex2i(500, 100);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	Color color = White;
	glColor4f(color.r, color.g, color.b, color.a);
	glEnd();
	glPopMatrix();



	// THIS https://stackoverflow.com/questions/24262264/drawing-a-2d-texture-in-opengl/24266568
	// https://stackoverflow.com/questions/30488155/opengl-fastest-way-to-draw-2d-image

	//ImGui::Image((ImTextureID)material->tex_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

	/*glPushMatrix();
	glLoadIdentity();

	glTranslatef(position2D.x, position2D.y, pos.z);
	glMultTransposeMatrixf(camera->origin_view_matrix);

	glPopMatrix();*/
}

json Image::Save() const
{
	json node;
	return node;
}

void Image::Load(json& node)
{
}

void Image::CreateInspectorNode()
{
	ImGui::Checkbox("##ImageActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Image"))
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::Checkbox("Visible", &visible);
		ImGui::Separator();

		// Size
		ImGui::Text("Size:    ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##imagesize", &size2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##imagesize", &size2D.y);

		// Position
		ImGui::Text("Position:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("x##imageposition", &position2D.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##imageposition", &position2D.y);

		// Rotation
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("##imagerotation", &rotation2D);

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
					texture = (ResourceTexture*)App->resources->GetResource(UID);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Separator();
		ImGui::Separator();
		ImGui::TreePop();
	}

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - 100);

	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		GO->RemoveComponent(Component::ComponentType::Image);
	}
}