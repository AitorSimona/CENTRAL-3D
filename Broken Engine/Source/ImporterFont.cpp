#include "ImporterFont.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ResourceFont.h"
#include "ImporterMeta.h"
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

	font->Init();

	App->ui_system->fonts.push_back(font);

	

	return font;
}

void ImporterFont::Save(ImporterFont* anim) const
{
}

Resource* ImporterFont::Load(const char* path) const
{

	return nullptr;
}
