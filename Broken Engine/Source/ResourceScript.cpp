#include "ResourceScript.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleScripting.h"

ResourceScript::ResourceScript(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCRIPT, UID, source_file)
{
	extension = ".lua";
	resource_file = SCRIPTS_FOLDER + std::to_string(UID) + extension;
}

ResourceScript::~ResourceScript()
{
}

bool ResourceScript::LoadInMemory()
{
	//Script doesn't generate any additional memory than the resource cretion itself
	return true;
}

void ResourceScript::OnOverwrite()
{

	App->scripting->NotifyHotReloading();
	// Here we have to manage hot Reloading
}

void ResourceScript::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	//FreeMemory();
	if(App->fs->Exists(resource_file.c_str()))
		App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceScript::FreeMemory()
{
}
