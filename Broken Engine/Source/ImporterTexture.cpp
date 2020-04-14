#include "ImporterTexture.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ImporterTexture::ImporterTexture() : Importer(Importer::ImporterType::Texture) 
{
}

ImporterTexture::~ImporterTexture() 
{
}

Resource* ImporterTexture::Import(ImportData& IData) const 
{
	if (!App->fs->Exists(IData.path))
		return nullptr;

	ResourceTexture* texture = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE, IData.path);;

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, texture->GetOriginalFile(), texture->GetUID());

	if (meta)
		IMeta->Save(meta);


	Save(texture);

	return texture;
}

Resource* ImporterTexture::Load(const char* path) const 
{
	ResourceTexture* texture = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	texture = App->resources->textures.find(meta->GetUID()) != App->resources->textures.end() ? (ResourceTexture*)App->resources->GetResource(meta->GetUID()) : (ResourceTexture*)App->resources->CreateResourceGivenUID(Resource::ResourceType::TEXTURE, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(texture->GetOriginalFile())) 
	{
		texture->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(texture);
	}

	return texture;
}

void ImporterTexture::Save(ResourceTexture* texture) const 
{
	// Nothing to save ? (being saved by ModuleTextures)
}
