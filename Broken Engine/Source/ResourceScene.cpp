#include "ResourceScene.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"


#include "mmgr/mmgr.h"


ResourceScene::ResourceScene(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCENE, UID, source_file)
{
	extension = ".scene";
	resource_file = SCENES_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceScene::~ResourceScene()
{
}

bool ResourceScene::LoadInMemory()
{
	return true;

}

void ResourceScene::FreeMemory()
{
}

void ResourceScene::OnOverwrite()
{
}

void ResourceScene::OnDelete()
{
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
