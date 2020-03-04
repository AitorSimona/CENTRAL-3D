#include "ImporterMeta.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

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
	ResourceMeta* resource = nullptr;

	std::string meta = path;
	meta.append(".meta");

	if (!App->fs->Exists(meta.c_str()))
		return nullptr;

	// --- Load meta file ---
	json file = App->GetJLoader()->Load(meta.c_str());
	json UID_node = file["UID"];
	json source_file = file["SOURCE"];
	json Date = file["DATE"];
	json fileFormatVersion = file["fileFormatVersion"];

	// --- Check for nulls ---
	if (source_file.is_null() || UID_node.is_null() || Date.is_null())
		return nullptr;

	uint UID = 0;

	// --- Retrieve UID ---
	if (App->resources->GetFileFormatVersion() == 2) // EXAMPLE of usage of deprecated functionality 
		UID = UID_node.get<uint>();
	else
		UID = std::stoi(UID_node.get<std::string>());

	// Date is retrieved on resource meta constructor
	resource = App->resources->metas.find(UID) != App->resources->metas.end() ? App->resources->metas.find(UID)->second : (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, source_file.get<std::string>(), UID);

	// --- Fill meta ---
	if (resource)
	{
		if (!fileFormatVersion.is_null())
			resource->fileFormatVersion = fileFormatVersion.get<uint>();
	}

	// --- A folder has been renamed ---
	if (!App->fs->Exists(source_file.get<std::string>().c_str()))
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
	jsonmeta["UID"] = meta->GetUID();
	jsonmeta["DATE"] = meta->Date;
	jsonmeta["SOURCE"] = meta->GetOriginalFile();
	jsonmeta["fileFormatVersion"] = App->resources->fileFormatVersion; // make sure resources have saved in newest version
	jsondata = App->GetJLoader()->Serialize(jsonmeta);
	meta_buffer = (char*)jsondata.c_str();

	App->fs->Save(meta->GetResourceFile(), meta_buffer, jsondata.length());

}
