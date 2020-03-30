#include <fstream>
#include <iomanip>

#include "ImporterShader.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ResourceShader.h"



#include "mmgr/mmgr.h"

using namespace Broken;
ImporterShader::ImporterShader() : Importer(Importer::ImporterType::Shader) 
{
}

ImporterShader::~ImporterShader() 
{
}

Resource* ImporterShader::Import(ImportData& IData) const 
{
	if (!App->fs->Exists(IData.path))
		return nullptr;

	ResourceShader* shader = (ResourceShader*)App->resources->CreateResource(Resource::ResourceType::SHADER, IData.path);;

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, shader->GetOriginalFile(), shader->GetUID());

	if (meta)
		IMeta->Save(meta);


	shader->LoadInMemory();
	Save(shader);

	return shader;
}

Resource* ImporterShader::Load(const char* path) const
{
	ResourceShader* shader = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	shader = App->resources->shaders.find(meta->GetUID()) != App->resources->shaders.end() ? App->resources->shaders.find(meta->GetUID())->second : (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(shader->GetOriginalFile()))
	{
		shader->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(shader);
	}

	return shader;
}


void ImporterShader::Save(ResourceShader* shader) const 
{
	GLint buffer_size;
	glGetProgramiv(shader->ID, GL_PROGRAM_BINARY_LENGTH, &buffer_size);
	
	if (buffer_size > 0)
	{
		// --- Create binary ---
		char* buffer = new char[buffer_size];
		GLint bytes_written = 0;
		GLenum format = 0;
	
		glGetProgramBinary(shader->ID, buffer_size, &bytes_written, &format, buffer);
	
		if (bytes_written > 0)
		{
			// --- Save shader to binary ---
			App->fs->Save(shader->GetResourceFile(), buffer, buffer_size);

			// --- Save shader code, update meta ---
			json jsonfile;
	
			std::ofstream file;
			file.open(shader->GetOriginalFile(), std::ofstream::out | std::ofstream::trunc);
	
			if (!file.is_open())
			{
				ENGINE_CONSOLE_LOG("|[error]: JSONLoader::Save could not open File: %s", shader->GetOriginalFile());
			}
			else
			{
				// --- Build shader code and save to file---
				//file << std::setw(5) << "#if VERTEX_SHADER" << std::endl;
				file << std::setw(5) << shader->vShaderCode << std::endl;

				//file << std::setw(5) << "#elseif FRAGMENT_SHADER" << std::endl;
				std::string tmp = shader->fShaderCode;
				uint loc = tmp.find("#define FRAGMENT_SHADER");

				if (loc != std::string::npos)
				{
					tmp = tmp.substr(loc, tmp.size());
				}

				file << std::setw(5) << tmp ;

				//file << std::setw(5) << "#endif" << std::endl;

				file.close();

				// --- Update meta ---
				ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
				ResourceMeta* meta = (ResourceMeta*)IMeta->Load(shader->GetOriginalFile());
				jsonfile["FORMAT"] = format;

				// --- Create Meta ---
				if (!meta)
					meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, shader->GetOriginalFile(), shader->GetUID());

				// --- Add shader data to meta ---
				if (meta)
				{
					meta->ResourceData = jsonfile;
					IMeta->Save(meta);
				}
			}
		}
	
		delete[] buffer;
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Could not save Shader: %s, try compiling it first", shader->GetName());
}


