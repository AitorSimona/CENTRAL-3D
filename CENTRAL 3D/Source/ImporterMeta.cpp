#include "ImporterMeta.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "ResourceMeta.h"
#include "JSONLoader.h"

#include "mmgr/mmgr.h"


ImporterMeta::ImporterMeta() : Importer(Importer::ImporterType::Meta)
{
}

ImporterMeta::~ImporterMeta()
{
}

Resource* ImporterMeta::Import(ImportData& IData) const
{

	return nullptr;
}

Resource* ImporterMeta::Load(const char* path) const
{
	Resource* resource = nullptr;

	std::string meta = path;
	meta.append(".meta");

	if (!App->fs->Exists(meta.c_str()))
		return nullptr;

	// --- Load meta file ---
	json file = App->GetJLoader()->Load(meta.c_str());
	uint UID = std::stoi(file["UID"].get<std::string>());
	std::string source_file = file["SOURCE"].get<std::string>();

	// Date is retrieved on resource meta constructor
	resource = App->resources->metas.find(UID) != App->resources->metas.end() ? App->resources->metas.find(UID)->second : App->resources->CreateResourceGivenUID(Resource::ResourceType::META, source_file, UID);

	// --- A folder has been renamed ---
	if (!App->fs->Exists(source_file.c_str()))
	{
		resource->SetOriginalFile(path);
	}

	return resource;
}

void ImporterMeta::Save(ResourceMeta * meta) const
{
	json jsonmeta;
	std::string jsondata;
	char* meta_buffer = nullptr;

	// --- Create Meta ---
	jsonmeta["UID"] = std::to_string(meta->GetUID());
	jsonmeta["DATE"] = std::to_string(meta->Date);
	jsonmeta["SOURCE"] = meta->GetOriginalFile();
	jsondata = App->GetJLoader()->Serialize(jsonmeta);
	meta_buffer = (char*)jsondata.c_str();

	App->fs->Save(meta->GetResourceFile(), meta_buffer, jsondata.length());

	// --- Hide meta ---
	App->fs->HideFile(meta->GetResourceFile());
}
