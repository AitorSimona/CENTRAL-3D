#include "ComponentImage.h"
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

#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentCanvas.h"
//#include "ModuleWindow.h"

#include "ResourceShader.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentImage::ComponentImage(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Image)
{
	name = "Image";
	visible = true;

	canvas = (ComponentCanvas*)gameObject->AddComponent(Component::ComponentType::Canvas);
	//texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, "DefaultTexture");
	canvas->AddElement(this);
}

ComponentImage::~ComponentImage()
{
	if (texture)
	{
		texture->Release();
		texture->RemoveUser(GO);
	}
}

void ComponentImage::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentImage::Draw()
{
	// --- Update transform and rotation to face camera ---
	float3 position = App->renderer3D->active_camera->frustum.NearPlanePos(-1, -1);
	float4x4 transform = transform.FromTRS(position, App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(), float3(size2D * 0.01f, 1.0f));

	// --- Set Uniforms ---
	uint shaderID = App->renderer3D->defaultShader->ID;
	glUseProgram(shaderID);

	GLint modelLoc = glGetUniformLocation(shaderID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(shaderID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();
	
	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		position2D.x * 0.01f, position2D.y * 0.01f, nearp - 0.05f, 0.0f);

	GLint projectLoc = glGetUniformLocation(shaderID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	// --- Color & Texturing ---
	glUniform3f(glGetUniformLocation(shaderID, "u_Color"), 1.0f, 1.0f, 1.0f);
	int TextureLocation = glGetUniformLocation(shaderID, "u_UseTextures");
	
	if (texture)
	{
		glUniform1i(TextureLocation, 1);
		glUniform1i(glGetUniformLocation(shaderID, "u_AlbedoTexture"), 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texture->GetTexID());
	}
	else
		glUniform1i(TextureLocation, 0);

	// --- Draw plane with given texture ---
	glBindVertexArray(App->scene_manager->plane->VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->plane->EBO);
	glDrawElements(GL_TRIANGLES, App->scene_manager->plane->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glUniform1i(TextureLocation, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
}

json ComponentImage::Save() const
{
	json node;
	node["Active"] = this->active;
	node["visible"] = std::to_string(visible);

	node["Resources"]["ResourceTexture"];

	if (texture)
		node["Resources"]["ResourceTexture"] = std::string(texture->GetResourceFile());

	node["position2Dx"] = std::to_string(position2D.x);
	node["position2Dy"] = std::to_string(position2D.y);

	node["size2Dx"] = std::to_string(size2D.x);
	node["size2Dy"] = std::to_string(size2D.y);



	return node;
}

void ComponentImage::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];
	std::string visible_str = node["visible"].is_null() ? "0" : node["visible"];
	visible = bool(std::stoi(visible_str));

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

	position2D = float2(std::stof(position2Dx), std::stof(position2Dy));
	size2D = float2(std::stof(size2Dx), std::stof(size2Dy));
}

void ComponentImage::CreateInspectorNode()
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	ImGui::Checkbox("Visible", &visible);
	ImGui::Separator();

	// Size
	ImGui::Text("Size:    ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	if (ImGui::DragFloat("x##imagesize", &size2D.x, 0.01f) && resize)
	{
		if (texture)
		{
			if (texture->Texture_height != 0 && texture->Texture_width != 0)
			{
				if (texture->Texture_width <= texture->Texture_height)
					size2D.y = size2D.x * (float(texture->Texture_width) / float(texture->Texture_height));
				else
					size2D.y = size2D.x * (float(texture->Texture_height) / float(texture->Texture_width));
			}
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	if (ImGui::DragFloat("y##imagesize", &size2D.y, 0.01f) && resize)
	{
		if (texture)
		{
			if (texture->Texture_height != 0 && texture->Texture_width != 0)
			{
				if (texture->Texture_width >= texture->Texture_height)
					size2D.x = size2D.y * (float(texture->Texture_width) / float(texture->Texture_height));
				else
					size2D.x = size2D.y * (float(texture->Texture_height) / float(texture->Texture_width));
			}
		}
	}

	// Position
	ImGui::Text("Position:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("x##imageposition", &position2D.x, 0.1f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(60);
	ImGui::DragFloat("y##imageposition", &position2D.y, 0.1f);

	// Rotation
	//ImGui::Text("Rotation:");
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(60);
	//ImGui::DragFloat("##imagerotation", &rotation2D);

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

				if (resize && texture)
					size2D = float2(texture->Texture_width, texture->Texture_height);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Aspect Ratio
	ImGui::Checkbox("Maintain Aspect Ratio", &resize);

	ImGui::Separator();
}