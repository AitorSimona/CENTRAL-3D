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
	Resource* animator = Load(IData.path);

	return animator;
}


void ImporterAnimator::Save(ResourceAnimator* animator) const
{
	
}


Resource* ImporterAnimator::Load(const char* path) const
{
	ResourceAnimator* animator = nullptr;

	// --- Load Scene file ---
	if (path) 
	{
		ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

		if (meta) 
			animator = App->resources->anim_info.find(meta->GetUID()) != App->resources->anim_info.end() ? App->resources->anim_info.find(meta->GetUID())->second : (ResourceAnimator*)App->resources->CreateResourceGivenUID(Resource::ResourceType::ANIMATOR, meta->GetOriginalFile(), meta->GetUID());
		else 
			animator = (ResourceAnimator*)App->resources->CreateResource(Resource::ResourceType::ANIMATOR, path);

	}

	return animator;
}
