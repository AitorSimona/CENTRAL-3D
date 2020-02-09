#include "ResourceMaterial.h"

#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"

#include "mmgr/mmgr.h"

ResourceMaterial::ResourceMaterial(uint UID, std::string source_file) : Resource(Resource::ResourceType::MATERIAL, UID, source_file)
{
	extension = ".mat";
	resource_file = source_file + extension;
	shader = App->renderer3D->defaultShader;
	previewTexID = App->gui->materialTexID;
}

ResourceMaterial::~ResourceMaterial()
{
	for (uint i = 0; i < uniforms.size(); ++i)
	{
		delete uniforms[i];
	}

	uniforms.clear();
}

bool ResourceMaterial::LoadInMemory()
{
	shader->GetAllUniforms(uniforms);

	return true;
}

void ResourceMaterial::FreeMemory()
{
}

void ResourceMaterial::UpdateUniforms()
{
	glUseProgram(shader->ID);

	for (uint i = 0; i < uniforms.size(); ++i)
	{

		switch (uniforms[i]->type)
		{
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

void ResourceMaterial::OnOverwrite()
{
}

void ResourceMaterial::OnDelete()
{
	App->fs->Remove(resource_file.c_str());
}

void ResourceMaterial::Repath()
{
	resource_file = original_file + extension;
}
