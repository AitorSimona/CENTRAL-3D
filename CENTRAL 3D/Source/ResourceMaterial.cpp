#include "ResourceMaterial.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "mmgr/mmgr.h"

ResourceMaterial::ResourceMaterial() : Resource(Resource::ResourceType::MATERIAL)
{
	shader = App->renderer3D->defaultShader;
	shader->instances++;

	shader->GetAllUniforms(uniforms);
}

ResourceMaterial::~ResourceMaterial()
{
	if(shader)
	shader->instances--;

	for (uint i = 0; i < uniforms.size(); ++i)
	{
		delete uniforms[i];
	}

	uniforms.clear();
}
