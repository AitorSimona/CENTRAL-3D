#include "ImporterShader.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "ResourceShader.h"

#include "mmgr/mmgr.h"

ImporterShader::ImporterShader() : Importer(Importer::ImporterType::Shader)
{
}

ImporterShader::~ImporterShader()
{
}

Resource* ImporterShader::Import(ImportData& IData) const
{

	bool ret = true;

	//if (data.vertexPath != nullptr || data.fragmentPath != nullptr)
	//{
	//	if (App->resources->IsFileImported(data.vertexPath) || App->resources->IsFileImported(data.fragmentPath))
	//	{
	//		// MYTODO: Should check if we are trying to import 2 times the same (check if resource exists)

	//		uint UID = App->resources->GetUIDFromMeta(data.vertexPath);

	//		std::string path = SHADERS_FOLDER;
	//		path.append(std::to_string(UID));
	//		path.append(".shader");

	//		char* buffer = nullptr;

	//		uint size = App->fs->Load(path.data(),&buffer);

	//		uint format;
	//		path.append(".format");
	//		std::string shader_name;

	//		if (App->fs->Exists(path.data()))
	//		{
	//			json file = App->GetJLoader()->Load(path.data());
	//			std::string formatstring = file["FORMAT"];
	//			std::string namestring = file["NAME"];
	//			shader_name = namestring;

	//			format = std::stoi(formatstring);
	//		}

	//		if (buffer)
	//		{
	//			// --- Load binary shader ---
	//			ResourceShader* shader = new ResourceShader(buffer, size, format, shader_name.data(), data.vertexPath, data.fragmentPath);
	//			shader->SetUID(UID);
	//			App->resources->AddResource(shader);
	//			App->resources->AddShader(shader);


	//			delete[] buffer;
	//		}
	//		else
	//			CONSOLE_LOG("|[error]: Could not load shader program");
	//	}

	//	else
	//	{


	//		ResourceShader* shader = new ResourceShader(data.vertexPath, data.fragmentPath);

	//		if (shader->ID != 0)
	//		{
	//			shader->name = data.vertexPath;
	//			uint count = shader->name.find_last_of("/");
	//			shader->name = shader->name.substr(count+1, shader->name.size());
	//			count = shader->name.find_last_of(".");
	//			shader->name = shader->name.substr(0,count);

	//			App->resources->AddResource(shader);
	//			App->resources->AddShader(shader);

	//			std::string path = SHADERS_FOLDER;
	//			path.append(std::to_string(shader->GetUID()));

	//			Save(shader, path.data());

	//			// --- Save metas ---
	//			std::string vertexmeta = data.vertexPath;
	//			std::string fragmentmeta = data.fragmentPath;

	//			App->resources->CreateMetaFromUID(shader->GetUID(), vertexmeta.data());
	//			App->resources->CreateMetaFromUID(shader->GetUID(), fragmentmeta.data());
	//		}
	//		else
	//			delete shader;

	//	}
	//}
	//else
	//	ret = false;

	return nullptr;
}

void ImporterShader::Save(ResourceShader * shader, const char * path) const
{
	//// --- Save Shaders to lib --- 
	//shader->Save();
}

Resource* ImporterShader::Load(const char * path) const 
{
	return nullptr;
}
