#include "ResourceMaterial.h"

#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleEventManager.h"

#include "ResourceTexture.h"

#include "ImporterMaterial.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceMaterial::ResourceMaterial(uint UID, const char* source_file) : Resource(Resource::ResourceType::MATERIAL, UID, source_file) 
{
	extension = ".mat";
	resource_file = source_file;
	shader = App->renderer3D->defaultShader;

	previewTexID = App->gui->materialTexID;

	shader->GetAllUniforms(uniforms);
}

ResourceMaterial::~ResourceMaterial() 
{

}

bool ResourceMaterial::LoadInMemory() 
{
	//shader->GetAllUniforms(uniforms);

	return true;
}

void ResourceMaterial::FreeMemory() 
{
	for (uint i = 0; i < uniforms.size(); ++i) 
	{
		delete uniforms[i];
	}

	uniforms.clear();
}

void ResourceMaterial::CreateInspectorNode()
{
	shader->GetAllUniforms(uniforms);
	DisplayAndUpdateUniforms();
	//UpdateUniforms();
}

void ResourceMaterial::UpdateUniforms() 
{
	glUseProgram(shader->ID);

	for (uint i = 0; i < uniforms.size(); ++i) {

		switch (uniforms[i]->type) {
		case GL_INT:
			glUniform1i(uniforms[i]->location, uniforms[i]->value.intU);
			break;

		case GL_FLOAT:
			glUniform1f(uniforms[i]->location, uniforms[i]->value.floatU);
			break;

		case GL_FLOAT_VEC2:
			glUniform2f(uniforms[i]->location, uniforms[i]->value.vec2U.x, uniforms[i]->value.vec2U.y);
			break;

		case GL_FLOAT_VEC3:
			glUniform3f(uniforms[i]->location, uniforms[i]->value.vec3U.x, uniforms[i]->value.vec3U.y, uniforms[i]->value.vec3U.z);
			break;

		case GL_FLOAT_VEC4:
			glUniform4f(uniforms[i]->location, uniforms[i]->value.vec4U.x, uniforms[i]->value.vec4U.y, uniforms[i]->value.vec4U.z, uniforms[i]->value.vec4U.w);
			break;

		case GL_INT_VEC2:
			glUniform2i(uniforms[i]->location, uniforms[i]->value.vec2U.x, uniforms[i]->value.vec2U.y);
			break;

		case GL_INT_VEC3:
			glUniform3i(uniforms[i]->location, uniforms[i]->value.vec3U.x, uniforms[i]->value.vec3U.y, uniforms[i]->value.vec3U.z);
			break;

		case GL_INT_VEC4:
			glUniform4i(uniforms[i]->location, uniforms[i]->value.vec4U.x, uniforms[i]->value.vec4U.y, uniforms[i]->value.vec4U.z, uniforms[i]->value.vec4U.w);
			break;
		}
	}

	glUseProgram(App->renderer3D->defaultShader->ID);
}

void ResourceMaterial::DisplayAndUpdateUniforms()
{
	// Note this is being done before any render happens

	static int tmp_int = 0;
	static float tmp_float = 0.0f;
	static float2 tmp_vec2 = { 0.0f,0.0f };
	static float3 tmp_vec3 = { 0.0f,0.0f,0.0f };
	static float4 tmp_vec4 = { 0.0f,0.0f,0.0f,0.0f };

	glUseProgram(shader->ID);

	bool updated = false;

	for (uint i = 0; i < uniforms.size(); ++i)
	{
		ImGui::PushID(GetUID() + i);
		ImGui::Text(uniforms[i]->name.data());
		ImGui::SameLine();

		switch (uniforms[i]->type)
		{
		case GL_INT:
			tmp_int = uniforms[i]->value.intU;

			if (ImGui::InputInt("##inputintuniform", &tmp_int, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.intU = tmp_int;
			}
			break;

		case GL_FLOAT:
			tmp_float = uniforms[i]->value.floatU;

			if (ImGui::InputFloat("##inputfloatuniform", &tmp_float, 1.0f, 100.0f, "%3f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.floatU = tmp_float;
			}

			break;

		case GL_FLOAT_VEC2:
			tmp_vec2 = uniforms[i]->value.vec2U;

			if (ImGui::InputFloat2("##inputfloat2uniform", tmp_vec2.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec2U = tmp_vec2;
			}

			break;

		case GL_FLOAT_VEC3:
			tmp_vec3 = uniforms[i]->value.vec3U;

			if (ImGui::InputFloat3("##inputfloat3uniform", tmp_vec3.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec3U = tmp_vec3;
			}

			break;

		case GL_FLOAT_VEC4:
			tmp_vec4 = uniforms[i]->value.vec4U;

			if (ImGui::InputFloat4("##inputfloat4uniform", tmp_vec4.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec4U = tmp_vec4;
			}

			break;

		case GL_INT_VEC2:
			tmp_vec2 = uniforms[i]->value.vec2U;

			if (ImGui::InputInt2("##inputint2uniform", (int*)tmp_vec2.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec2U = tmp_vec2;
			}
			break;

		case GL_INT_VEC3:
			tmp_vec3 = uniforms[i]->value.vec3U;

			if (ImGui::InputInt3("##inputint3uniform", (int*)tmp_vec3.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec3U = tmp_vec3;
			}
			break;

		case GL_INT_VEC4:
			tmp_vec4 = uniforms[i]->value.vec4U;

			if (ImGui::InputInt4("##inputint4uniform", (int*)tmp_vec4.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				updated = true;
				uniforms[i]->value.vec4U = tmp_vec4;
			}

			break;
		}

		ImGui::PopID();
	}

	if (updated)
	{
		UpdateUniforms();
		App->resources->GetImporter<ImporterMaterial>()->Save(this);
	}

	glUseProgram(App->renderer3D->defaultShader->ID);
}

void ResourceMaterial::OnOverwrite() {
	NotifyUsers(ResourceNotificationType::Overwrite);
}

void ResourceMaterial::OnDelete() {
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	Resource* diffuse = m_DiffuseResTexture;
	Resource* specular = m_SpecularResTexture;

	if (diffuse)
		diffuse->Release();

	if (specular)
		specular->Release();

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceMaterial::Repath() {
	resource_file = original_file + extension;
}