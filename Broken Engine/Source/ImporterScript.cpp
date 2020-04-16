#include "ImporterScript.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ResourceScript.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"

using namespace Broken;
ImporterScript::ImporterScript() : Importer(ImporterType::Script) 
{
}

ImporterScript::~ImporterScript() {
}

Resource* ImporterScript::Import(ImportData& IData) const {
	ResourceScript* resource_script = (ResourceScript*)App->resources->CreateResource(Resource::ResourceType::SCRIPT, IData.path);

	// Pass the relative path
	resource_script->relative_path = IData.path;

	// Construct absolute path (Lua virtual machine needs to have the absolute path of the script in order to compile it)
	std::string abs_path = App->fs->GetBasePath();

	std::size_t d_pos = 0;
	d_pos = abs_path.find("Debug");
	std::size_t r_pos = 0;
	r_pos = abs_path.find("Release");
	std::size_t g_pos = 0;
	g_pos = abs_path.find("Game");

	if (d_pos != 4294967295)  // If we are in DEBUG
	{
		abs_path = abs_path.substr(0,d_pos);
		abs_path += "Game/";
	}
	else if (r_pos != 4294967295) // If we are in RELEASE
	{
		abs_path = abs_path.substr(0,r_pos);
		abs_path += "Game/";
	}
	else if (g_pos != 4294967295) // If we are in a EXE final build
	{
		abs_path = abs_path.substr(0, g_pos);
	}
	
	abs_path += IData.path;
	App->fs->NormalizePath(abs_path);
	resource_script->absolute_path = abs_path.c_str();

	// Get Script name
	std::string file;
	App->fs->SplitFilePath(IData.path, nullptr, &file, nullptr);
	App->fs->RemoveFileExtension(file);
	resource_script->script_name = file;

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, resource_script->GetOriginalFile(), resource_script->GetUID());

	if (meta)
		IMeta->Save(meta);

	return (Resource*)resource_script;
}

Resource* ImporterScript::Load(const char* path) const {
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	uint UID = meta->GetUID();
	ResourceScript* resource_script = App->resources->scripts.find(UID) != App->resources->scripts.end() ? App->resources->scripts.find(UID)->second : (ResourceScript*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SCRIPT, meta->GetOriginalFile(), UID);

	//Pass the relative path
	resource_script->relative_path = path;

	//Construct absolute path (Lua virtual machine needs to have the absolute path of the script in order to compile it)
	std::string abs_path = App->fs->GetBasePath();

	std::size_t d_pos = 0;
	d_pos = abs_path.find("Debug");
	std::size_t r_pos = 0;
	r_pos = abs_path.find("Release");
	std::size_t g_pos = 0;
	g_pos = abs_path.find("Game");

	if (d_pos != 4294967295 && App->isGame == false)  // If we are in DEBUG
	{
		abs_path = abs_path.substr(0, d_pos);
		abs_path += "Game/";
	}

	if (r_pos != 4294967295 && App->isGame == false) // If we are in RELEASE
	{
		abs_path = abs_path.substr(0, r_pos);
		abs_path += "Game/";

		int i = 100;
		assert(i != 100, "We detected we are in an exe!");
	}

	//if (g_pos != 4294967295) // If we are in a EXE final build
	//{
	//	//abs_path = abs_path.substr(0, g_pos);
	//}
	
	abs_path += path;
	App->fs->NormalizePath(abs_path);
	resource_script->absolute_path = abs_path.c_str();

	//Get Script name
	std::string file;
	App->fs->SplitFilePath(path, nullptr, &file, nullptr);
	App->fs->RemoveFileExtension(file);
	resource_script->script_name = file;

	return (Resource*)resource_script;
}

void ImporterScript::Save(ResourceScript* script)
{
	char* data = new char[1]; // Allocate
	uint size = 1;

	App->fs->Save(script->GetOriginalFile(), data, size);

	if (data)
	{
		delete[] data;
		data = nullptr;
	}

	// --- Update meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(script->GetOriginalFile());

	if (meta)
	{
		meta->Date = App->fs->GetLastModificationTime(script->GetOriginalFile());
		IMeta->Save(meta);
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Could not load meta from: %s", script->GetResourceFile());
}
