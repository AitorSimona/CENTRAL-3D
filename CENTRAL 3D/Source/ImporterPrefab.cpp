#include "ImporterPrefab.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ResourcePrefab.h"

#include "mmgr/mmgr.h"


ImporterPrefab::ImporterPrefab() : Importer(Importer::ImporterType::Prefab)
{
}

ImporterPrefab::~ImporterPrefab()
{
}

Resource* ImporterPrefab::Import(ImportData& IData) const
{
	return nullptr;
}

Resource* ImporterPrefab::Load(const char* path) const
{
	ResourcePrefab* prefab = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	prefab = App->resources->textures.find(meta->GetUID()) != App->resources->textures.end() ? (ResourcePrefab*)App->resources->GetResource(meta->GetUID()) : (ResourcePrefab*)App->resources->CreateResourceGivenUID(Resource::ResourceType::PREFAB, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(prefab->GetOriginalFile()))
	{
		prefab->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(prefab);
	}

	return prefab;
}

void ImporterPrefab::Save(ResourcePrefab* texture) const
{
}
