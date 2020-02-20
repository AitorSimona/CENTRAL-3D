#include "ImporterFolder.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "ImporterMeta.h"

#include "ResourceMeta.h"
#include "ResourceFolder.h"

#include "mmgr/mmgr.h"


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

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	// --- Create meta ---
	std::string new_path = IData.path;
	new_path.pop_back();
	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, new_path, folder->GetUID());

	if (meta)
		IMeta->Save(meta);

	App->resources->AddResourceToFolder(folder);

	return folder;
}

Resource* ImporterFolder::Load(const char* path) const
{
	ResourceFolder* folder = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	// --- Load meta first ---
	std::string new_path = path;
	new_path.pop_back();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(new_path.c_str());

	folder = App->resources->folders.find(meta->GetUID()) != App->resources->folders.end() ? App->resources->folders.find(meta->GetUID())->second : (ResourceFolder*)App->resources->CreateResourceGivenUID(Resource::ResourceType::FOLDER, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(folder->GetOriginalFile()))
	{
		folder->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(folder);
	}

	return folder;
}

void ImporterFolder::Save(ResourceFolder* folder) const
{
}
