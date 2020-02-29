#include "Image.h"
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

#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
//#include "ModuleWindow.h"

#include "ResourceShader.h"
#include "ResourceTexture.h"

#include "Math.h"
#include "ResourceMesh.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

Image::Image(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Image)
{
	visible = true;

	canvas = (Canvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "DefaultTexture");
	canvas->AddElement(this);
}

Image::~Image()
{
	if(texture)
	texture->Release();
}

void Image::Draw()
{
	// --- Update transform and rotation to face camera ---

	rotation2D *= DEGTORAD;

	float3 frustum_pos = App->renderer3D->active_camera->frustum.Pos();
	float3 center = float3(frustum_pos.x, frustum_pos.y, 10); // si metes el position aqui no deja moverla

	// --- Frame image with camera ---
	float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, 10), // si metes el position aqui si que se mueve (se carga el billboarding)
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(), float3(size2D, 1));
	
	transform.SetTranslatePart(transform.TranslatePart().x + position2D.x, transform.TranslatePart().y + position2D.y, 10); // esto mueve el plano en sus ejes X e Y (se carga el billboarding)
	transform.SetRotatePartZ(rotation2D); // esto funciona (rotar imagen en su eje Z)
	
	float3 Movement = App->renderer3D->active_camera->frustum.Front();
	float3 camera_pos = frustum_pos;
	
	if (Movement.IsFinite())
		App->renderer3D->active_camera->frustum.SetPos(center - Movement);

	rotation2D *= RADTODEG;

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->defaultShader->ID);

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
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());


	// --- Draw plane with given texture ---
	glBindVertexArray(App->scene_manager->plane->VAO);

	glBindTexture(GL_TEXTURE_2D, texture->GetTexID());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->plane->EBO);
	glDrawElements(GL_TRIANGLES, App->scene_manager->plane->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)


	// --- Set camera back to original position ---
	App->renderer3D->active_camera->frustum.SetPos(camera_pos);
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
		ImGui::DragFloat("x##imageposition", &position2D.x, 0.01f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("y##imageposition", &position2D.y, 0.01f);

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
				{
					if (texture)
						texture->Release();

					texture = (ResourceTexture*)App->resources->GetResource(UID);
				}
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