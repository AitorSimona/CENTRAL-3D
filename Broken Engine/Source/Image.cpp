#include "Image.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleTextures.h"
#include "PanelScene.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"

#include "ModuleSceneManager.h"

#include "ComponentCamera.h"

#include "ResourceShader.h"

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
	if(texture)
	texture->Release();
}

void Image::Draw()
{
	glTranslatef(position2D.x, position2D.y, 10.0);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture->GetTexID());
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(-size2D.x, -size2D.y);    // Top Left	glVertex2i(100, 100);
	glTexCoord2i(0, 1); glVertex2f(-size2D.x, size2D.y);    // Top Right	glVertex2i(100, 500);
	glTexCoord2i(1, 1); glVertex2f(size2D.x, size2D.y);    // Bottom Right	glVertex2i(500, 500);
	glTexCoord2i(1, 0); glVertex2f(size2D.x, -size2D.y);    // Bottom Left	glVertex2i(500, 100);

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

	////--------------
	////glPushMatrix();
	////glLoadIdentity();


	//float4x4 transform = float4x4::identity;
	//transform.FromTRS(float3(position2D, 10), Quat(), float3(size2D,1));
	//// --- Set Uniforms ---
	//glUseProgram(App->renderer3D->linepointShader->ID);

	//GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	//GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	//float nearp = App->renderer3D->active_camera->GetNearPlane();

	//// right handed projection matrix
	//float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	//float4x4 proj_RH(
	//	f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
	//	0.0f, f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, -1.0f,
	//	0.0f, 0.0f, nearp, 0.0f);

	//GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");
	//glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	//float3* vertices = new float3[4];

	//vertices[0] = float3(-100, 0, 0);
	//vertices[1]= float3(100, 0,0);
	//vertices[2]= float3(-100, 0,0);
	//vertices[3]= float3(-100, 100,0);


	//// --- Create VAO, VBO ---
	//unsigned int VBO;
	//glGenBuffers(1, &VBO);
	//// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	//glBindVertexArray(App->scene_manager->GetPointLineVAO());

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4, vertices, GL_DYNAMIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//// --- Draw lines ---
	//glLineWidth(3.0f);
	//glBindVertexArray(App->scene_manager->GetPointLineVAO());
	//glDrawArrays(GL_LINES, 0, 4);
	//glBindVertexArray(0);
	//glLineWidth(1.0f);

	//// --- Delete VBO and vertices ---
	//glDeleteBuffers(1, &VBO);
	//delete[] vertices;

	//glUseProgram(App->renderer3D->defaultShader->ID);


	//////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	////glPushMatrix();
	////glMultMatrixf(float4x4::identity.Transposed().ptr());
	////glMatrixMode(GL_PROJECTION);
	////glLoadMatrixf(proj_RH.Transposed().ptr());
	////glMatrixMode(GL_MODELVIEW);
	////glLoadMatrixf(App->renderer3D->active_camera->GetOpenGLViewMatrix().Transposed().ptr());


	////glTranslatef(position2D.x, position2D.y, 1);
	////glMultTransposeMatrixf(camera->origin_view_matrix);


	////glBindTexture(GL_TEXTURE_2D, texture->GetTexID());
	////glEnable(GL_TEXTURE_2D);
	////glBegin(GL_QUADS);
	////// Draw A Quad
	////glTexCoord2i(0, 0); glVertex2f(-size2D.x, -size2D.y);    // Top Left	glVertex2i(100, 100);
	////glTexCoord2i(0, 1); glVertex2f(-size2D.x, size2D.y);    // Top Right	glVertex2i(100, 500);
	////glTexCoord2i(1, 1); glVertex2f(size2D.x, size2D.y);    // Bottom Right	glVertex2i(500, 500);
	////glTexCoord2i(1, 0); glVertex2f(size2D.x, -size2D.y);    // Bottom Left	glVertex2i(500, 100);

	//////glDisable(GL_TEXTURE_2D);
	////glBindTexture(GL_TEXTURE_2D, 0);

	////Color color = White;
	////glColor4f(color.r, color.g, color.b, color.a);
	////glEnd();
	////glPopMatrix();
	////glMatrixMode(GL_PROJECTION);
	////glLoadIdentity();
	////glMatrixMode(GL_MODELVIEW);
	////glLoadIdentity();
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