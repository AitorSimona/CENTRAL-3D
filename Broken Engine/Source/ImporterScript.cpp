#include "ImporterScript.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ResourceScript.h"

ImporterScript::ImporterScript() : Importer(ImporterType::Script)
{
}

ImporterScript::~ImporterScript()
{
}

Resource* ImporterScript::Import(ImportData& IData) const
{
	ResourceScript* resource_script = (ResourceScript*)App->resources->CreateResource(Resource::ResourceType::SCRIPT, IData.path);

	//Pass the relative path
	resource_script->relative_path = IData.path;

	//Construct absolute path (Lua virtual machine needs to have the absolute path of the script in order to compile it)
	std::string abs_path = App->fs->GetBasePath();

	std::size_t d_pos = 0;
	d_pos = abs_path.find("Debug");
	std::size_t r_pos = 0;
	r_pos = abs_path.find("Release");

	if (d_pos != 4294967295)  // If we are in DEBUG
	{
		abs_path = abs_path.substr(0,d_pos);
	}

	if (r_pos != 4294967295) // If we are in RELEASE
	{
		abs_path = abs_path.substr(0,r_pos);
	}

	abs_path += "Game/";
	abs_path += IData.path;
	App->fs->NormalizePath(abs_path);
	resource_script->absolute_path = abs_path.c_str();
	
	//Get Script name
	std::string file;
	App->fs->SplitFilePath(IData.path,nullptr,&file ,nullptr);
	App->fs->RemoveFileExtension(file);
	resource_script->script_name = file;

	return (Resource*)resource_script;
}

Resource* ImporterScript::Load(const char* path) const
{
	return nullptr;
}