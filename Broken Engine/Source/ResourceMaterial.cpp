#include "ResourceMaterial.h"

#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleEventManager.h"

#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceMaterial::ResourceMaterial(uint UID, const char* source_file) : Resource(Resource::ResourceType::MATERIAL, UID, source_file) {
	extension = ".mat";
	resource_file = source_file;
	shader = App->renderer3D->defaultShader;
	previewTexID = App->gui->materialTexID;
}

ResourceMaterial::~ResourceMaterial() {

}

bool ResourceMaterial::LoadInMemory() {
	shader->GetAllUniforms(uniforms);

	return true;
}

void ResourceMaterial::FreeMemory() {
	for (uint i = 0; i < uniforms.size(); ++i) {
		delete uniforms[i];
	}

	uniforms.clear();
}

void ResourceMaterial::UpdateUniforms() {
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

void ResourceMaterial::OnOverwrite() {
	NotifyUsers(ResourceNotificationType::Overwrite);
}

void ResourceMaterial::OnDelete() {
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	Resource* diffuse = resource_diffuse;

	if (diffuse)
		diffuse->Release();

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceMaterial::Repath() {
	resource_file = original_file + extension;
}