#include "ImporterFolder.h"
#include "Application.h"
#include "ModuleResources.h"

ImporterFolder::ImporterFolder() : Importer(Importer::ImporterType::Folder)
{
}

ImporterFolder::~ImporterFolder()
{
}

Resource* ImporterFolder::Import(ImportData& IData) const
{
	Resource* folder = nullptr;

	folder = App->resources->CreateResource(Resource::ResourceType::FOLDER, IData.path);

	return folder;
}

Resource* ImporterFolder::Load(const char* path) const
{
	return nullptr;
}

void ImporterFolder::Save(ResourceFolder* folder) const
{
}
