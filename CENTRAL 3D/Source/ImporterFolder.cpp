#include "ImporterFolder.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

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
	Resource* folder = nullptr;

	if (App->fs->Exists(path))
	{
		json file = App->GetJLoader()->Load(path);
		std::string uid = file["UID"];
		std::string source_file = path;

		source_file = source_file.substr(0, source_file.find_last_of("."));
		folder = App->resources->CreateResourceGivenUID(Resource::ResourceType::FOLDER, source_file, std::stoi(uid));
	}

	return folder;
}

void ImporterFolder::Save(ResourceFolder* folder) const
{
}
