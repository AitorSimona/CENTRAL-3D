#include "ImporterFont.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ResourceFont.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ModuleUI.h"
#include "mmgr/mmgr.h"


using namespace Broken;

ImporterFont::ImporterFont() : Importer(Importer::ImporterType::Font)
{
	
}

ImporterFont::~ImporterFont()
{
	
}

Resource* ImporterFont::Import(ImportData& IData) const
{
	ResourceFont* font = (ResourceFont*)App->resources->CreateResource(Resource::ResourceType::FONT, IData.path);

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, font->GetOriginalFile(), font->GetUID());

	if (meta)
		IMeta->Save(meta);


	if (font != nullptr)
	{
		font->Init();
	}

	return font;
}

void ImporterFont::Save(ImporterFont* anim) const
{
}

Resource* ImporterFont::Load(const char* path) const
{
	ResourceFont* font = nullptr;
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	font = App->resources->fonts.find(meta->GetUID()) != App->resources->fonts.end() ? App->resources->fonts.find(meta->GetUID())->second : (ResourceFont*)App->resources->CreateResourceGivenUID(Resource::ResourceType::FONT, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(font->GetOriginalFile())) {
		font->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(font);
	}
	
	if (font != nullptr)
	{
		font->Init();
	}

	return font;
}
