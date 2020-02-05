#include "ImporterFolder.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "ImporterMeta.h"

#include "ResourceMeta.h"

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

	return folder;
}

Resource* ImporterFolder::Load(const char* path) const
{
	Resource* folder = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	// --- Load meta first ---
	std::string new_path = path;
	new_path.pop_back();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(new_path.c_str());

	folder = App->resources->CreateResourceGivenUID(Resource::ResourceType::FOLDER, path, meta->GetUID());

	return folder;
}

void ImporterFolder::Save(ResourceFolder* folder) const
{
}
