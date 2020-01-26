#include "ImporterMeta.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "ResourceMeta.h"
#include "JSONLoader.h"

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
	// --- Load meta file ---
	json file = App->GetJLoader()->Load(path);
	// Date is retrieved on resource meta constructor
	return 	App->resources->CreateResourceGivenUID(Resource::ResourceType::META, file["SOURCE"].get<std::string>(), path, std::stoi(file["UID"].get<std::string>()));
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
}
