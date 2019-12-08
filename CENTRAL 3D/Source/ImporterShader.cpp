#include "ImporterShader.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "ResourceShader.h"

#include "mmgr/mmgr.h"

ImporterShader::ImporterShader()
{
}

ImporterShader::~ImporterShader()
{
}

bool ImporterShader::Import(const ImportData & IData) const
{
	ImportShaderData data = (ImportShaderData&)IData;

	bool ret = true;

	if (data.vertexPath != nullptr || data.fragmentPath != nullptr)
	{
		ResourceShader* shader = new ResourceShader(data.vertexPath, data.fragmentPath);

		if (shader->ID != 0)
			App->resources->AddResource(shader);
		else
			delete shader;
	}
	else
		ret = false;

	return ret;
}

void ImporterShader::Save(ResourceShader * shader, const char * path) const
{
}

void ImporterShader::Load(const char * filename, ResourceShader & shader) const
{
}
