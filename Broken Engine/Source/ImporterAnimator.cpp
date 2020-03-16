#include "ImporterAnimator.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceAnimator.h"
#include "ResourceMeta.h"
#include "ImporterMeta.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ImporterAnimator::ImporterAnimator() : Importer(Importer::ImporterType::Animator)
{
}

ImporterAnimator::~ImporterAnimator()
{
}

Resource* ImporterAnimator::Import(ImportData& IData) const
{
	if (!App->fs->Exists(IData.path))
		return nullptr;

	ResourceAnimator* animator = (ResourceAnimator*)App->resources->CreateResource(Resource::ResourceType::ANIMATOR, IData.path);;

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, animator->GetOriginalFile(), animator->GetUID());

	if (meta)
		IMeta->Save(meta);

	//Save(animator);

	return animator;
}


void ImporterAnimator::Save(ResourceAnimator* animator) const
{
	
}


Resource* ImporterAnimator::Load(const char* path) const
{
	ResourceAnimator* animator = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	animator = App->resources->anim_info.find(meta->GetUID()) != App->resources->anim_info.end() ? App->resources->anim_info.find(meta->GetUID())->second : (ResourceAnimator*)App->resources->CreateResourceGivenUID(Resource::ResourceType::ANIMATOR, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(animator->GetOriginalFile())) 
	{
		animator->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(animator);
	}

	return animator;
}
